//
// Created by cervi on 16/10/2023.
//
#include "wav.hpp"

namespace Audio
{
    void WAV::load(const std::string &name)
    {
        if (name.empty()) // Loading an empty WAV is the same as freeing it
            return;
        free_();

        _loops = 0;
        std::string realPath = "nitro:/" + name + ".wav";
        FILE *f = fopen(realPath.c_str(), "rb");
        _filename = name;
        if (f == nullptr)
        {
            std::string buffer = "Error opening WAV #r" + name;
            return;
        }

        char header[4];

        const char riffHeader[4] = {'R', 'I', 'F', 'F'};
        const char waveHeader[4] = {'W', 'A', 'V', 'E'};
        const char fmtHeader[4] = {'f', 'm', 't', ' '};
        const char dataHeader[4] = {'d', 'a', 't', 'a'};

        fread(header, 4, 1, f);
        if (memcmp(header, riffHeader, 4) != 0)
        {
            std::string buffer = "Error opening WAV #r" + name +
                                 "#x: Invalid RIFF header.";
            fclose(f);
            return;
        }

        u32 fileSize;
        fread(&fileSize, 4, 1, f);

        fread(header, 4, 1, f);
        if (memcmp(header, waveHeader, 4) != 0)
        {
            std::string buffer = "Error opening WAV #r" + name +
                                 "#x: Invalid WAVE header.";
            fclose(f);
            return;
        }

        // fmt header
        fread(header, 4, 1, f);
        if (memcmp(header, fmtHeader, 4) != 0)
        {
            std::string buffer = "Error opening WAV #r" + name +
                                 "#x: Invalid FMT header.";
            fclose(f);
            return;
        }

        u32 chunkSize = 0;
        fread(&chunkSize, 4, 1, f); // skip chunk size == 0x10
        u32 chunkEnd = ftell(f) + chunkSize;

        u16 format, channels, bitsPerSample;
        u32 sampleRate;
        fread(&format, 2, 1, f);
        fread(&channels, 2, 1, f);
        fread(&sampleRate, 4, 1, f);
        _sampleRate = sampleRate;
        fseek(f, 4, SEEK_CUR);        // skip byte rate == self.sample_rate * self.bits_per_sample * self.num_channels // 8
        fread(&_blockAlign, 2, 1, f); // TODO: Needed to be taken into account when decoding PCM?
        fread(&bitsPerSample, 2, 1, f);
        _bitsPerSample = bitsPerSample;

        if (format == 1)
        {
            if (_bitsPerSample == 8)
            {
                _format = SoundFormat_8Bit;
            }
            else if (_bitsPerSample == 16)
            {
                _format = SoundFormat_16Bit;
            }
            else
            {
                std::string buffer = "Error opening WAV #r" + name +
                                     "#x: Invalid bits per sample.";
                fclose(f);
                return;
            }
        }
        else if (format == 0x11)
        {
            _format = SoundFormat_ADPCM;
            if (_bitsPerSample != 4)
            {
                std::string buffer = "Error opening WAV #r" + name +
                                     "#x: Invalid bits per sample.";
                fclose(f);
                return;
            }
        }
        else
        {
            std::string buffer = "Error opening WAV #r" + name +
                                 "#x: Invalid format.";
            fclose(f);
            return;
        }

        if (channels > 2)
        {
            std::string buffer = "Error opening WAV #r" + name +
                                 "#x: Invalid channels.";
            fclose(f);
            return;
        }

        _stereo = channels == 2;

        fseek(f, chunkEnd, SEEK_SET);

        // data chunk
        while (ftell(f) < fileSize + 8)
        {
            fread(header, 4, 1, f);
            fread(&chunkSize, 4, 1, f);
            if (memcmp(header, dataHeader, 4) == 0)
                break;
            fseek(f, chunkSize, SEEK_CUR);
        }
        if (chunkSize == 0)
        {
            std::string buffer = "Error opening WAV #r" + name +
                                 "#x: Couldn't find DATA chunk.";
            fclose(f);
            return;
        }

        _dataEnd = ftell(f) + chunkSize;
        _dataStart = ftell(f);

        allocateBuffers();

        _loaded = true;
        _stream = f;
    }

    void WAV::resetPlaying()
    {
        fseek(_stream, _dataStart, SEEK_SET);
        _sourceBufferPos = 0;
        _sampleBufferPos = 0;
        _expectedSampleBufferPos = 0;
        _fileBufferSamplePos = 0;
        _fileBufferSampleEnd = 0;
    }

    WAV::~WAV()
    {
        free_();
    }

    void WAV::free_()
    {
        if (_active)
            stop();

        if (_stream != nullptr)
        {
            fclose(_stream);
            _stream = nullptr;
        }
    }

    SoundFormat WAV::getAllocFormat()
    {
        if (_format == SoundFormat_ADPCM)
            return SoundFormat_16Bit;
        return _format;
    }

    u8 WAV::getBitsPerSample()
    {
        if (_format == SoundFormat_ADPCM)
            return 16;
        return _bitsPerSample;
    }

    void WAV::progress(u16 samples)
    {
        while (samples > 0)
        {
            u32 remainingFileBuffer = _fileBufferSampleEnd - _fileBufferSamplePos;
            u32 remainingLeftBuffer = kAudioBuffer - _sampleBufferPos % kAudioBuffer;
            u32 max_copy = remainingFileBuffer < remainingLeftBuffer ? remainingFileBuffer : remainingLeftBuffer;
            max_copy = samples < max_copy ? samples : max_copy;

            copy_from_file_buffer(max_copy);

            _fileBufferSamplePos += max_copy;
            _sampleBufferPos += max_copy;
            _sourceBufferPos += max_copy;

            if (_fileBufferSamplePos >= _fileBufferSampleEnd)
            {
                if (renew_file_buffer())
                {
                    nocashMessage("Completed");
                    // We should stop the audio if we have completed.
                    if (_expectedSampleBufferPos > _sampleBufferPos)
                        stop();
                    break;
                }
            }

            samples -= max_copy;
        }
    }

    void WAV::copy_from_file_buffer(u16 copy_length_samples)
    {
        if (!_stereo)
        {
            switch (_format)
            {
            // case SoundFormat_8Bit:
            //     progress_pcm8_mono(copy_length_samples);
            //     break;
            // case SoundFormat_16Bit:
            //     progress_pcm16_mono(copy_length_samples);
            //     break;
            case SoundFormat_ADPCM:
                progress_ima_adpcm_mono(copy_length_samples);
                break;
            default:
                printf("WAV: Format not implemented");
                return;
            }
        }
        else
        {
            switch (_format)
            {
            // case SoundFormat_8Bit:
            //     progress_pcm8_stereo(copy_length_samples);
            //     break;
            // case SoundFormat_16Bit:
            //     progress_pcm16_stereo(copy_length_samples);
            //     break;
            case SoundFormat_ADPCM:
                progress_ima_adpcm_stereo(copy_length_samples);
                break;
            default:
                printf("WAV: Format not implemented");
                return;
            }
        }
    }

    bool WAV::renew_file_buffer()
    {
        if (ftell(_stream) >= _dataEnd)
        {
            if (_loops == 0)
            {
                return true;
            }
            else if (_loops > 0)
            {
                _loops--;
            }
            fseek(_stream, _dataStart, SEEK_SET);
            _sourceBufferPos = 0;
        }

        u32 maxReadSize = _dataEnd - ftell(_stream);
        if (maxReadSize >= kAudioBuffer * 2)
            maxReadSize = kAudioBuffer * 2;
        fread(_fileBuffer, maxReadSize, 1, _stream);
        _fileBufferSamplePos = 0;

        // Calculate _fileBufferSampleEnd
        if (!_stereo)
        {
            switch (_format)
            {
            // case SoundFormat_8Bit:
            //     _fileBufferSampleEnd = maxReadSize;
            //     break;
            // case SoundFormat_16Bit:
            //     _fileBufferSampleEnd = maxReadSize / 2;
            //     break;
            case SoundFormat_ADPCM:
            {
                u32 samplesPerBlock = (_blockAlign * 2) - 7;
                _fileBufferSampleEnd = (maxReadSize * samplesPerBlock) / (u32)_blockAlign;
                break;
            }
            default:
                printf("WAV: Format (2) not impl");
                break;
                ;
            }
        }
        else
        {
            switch (_format)
            {
            // case SoundFormat_8Bit:
            //     _fileBufferSampleEnd = maxReadSize / 2;
            //     break;
            // case SoundFormat_16Bit:
            //     _fileBufferSampleEnd = maxReadSize / 4;
            //     break;
            case SoundFormat_ADPCM:
            {
                u32 samplesPerBlock = (_blockAlign * 2) / 2 - 7;
                _fileBufferSampleEnd = (maxReadSize * samplesPerBlock) / (u32)_blockAlign;
                break;
            }
            default:
                printf("WAV: Format (2) not implemented");
                break;
            }
        }
        return false;
    }

    // ADPCM
    void ADPCM_Decoder::initPredictor(int new_sample, int index)
    {
        _new_sample = new_sample;
        _index = index;
    }

    s16 ADPCM_Decoder::decodeSample(u8 inputNibble)
    {
        int step = kStepSizeTable[_index];
        int difference = step >> 3;

        if (inputNibble & 4)
            difference += step;
        if (inputNibble & 2)
            difference += step >> 1;
        if (inputNibble & 1)
            difference += step >> 2;

        if (inputNibble & 8)
            difference = -difference;

        _new_sample += difference;
        if (_new_sample > 32767)
            _new_sample = 32767;
        else if (_new_sample < -32768)
            _new_sample = -32768;

        _index += kIndexTable[inputNibble];
        if (_index < 0)
            _index = 0;
        else if (_index > 88)
            _index = 88;
        return _new_sample;
    }

    u32 WAV::ima_adpcm_get_sample_position(u32 sampleIdx, u8 channelIdx, bool &highBits,
                                           bool &readHeader)
    {
        u8 channels = _stereo ? 2 : 1;
        // Remove the 7 extra samples in the header
        u32 samplesPerBlock = _blockAlign * 2 / channels - 7;

        u32 block = sampleIdx / samplesPerBlock;
        u32 absOffset = block * _blockAlign;
        sampleIdx %= samplesPerBlock;

        if (sampleIdx == 0)
        {
            readHeader = true;
            return absOffset + channelIdx * 4;
        }
        else
        {
            readHeader = false;
        }
        sampleIdx -= 1;

        u32 channelBlock = sampleIdx / 8;
        sampleIdx %= 8;

        absOffset += ((channelBlock + 1) * channels + channelIdx) * 4;

        u8 sampleByte = sampleIdx / 2;
        highBits = sampleIdx & 1;

        absOffset += sampleByte;

        return absOffset;
    }

    void WAV::progress_ima_adpcm_mono(u16 samples)
    {
        if (_leftBuffer == nullptr)
            return;
        s16 *leftBuffer = reinterpret_cast<s16 *>(_leftBuffer.get());
        u8 *fileBuffer = _fileBuffer;
        for (int i = 0; i < samples; i++)
        {
            bool highBits, readHeader;
            u32 samplePos;
            samplePos = ima_adpcm_get_sample_position(
                _sourceBufferPos + i, 0,
                highBits, readHeader);
            if (readHeader)
            {
                u16 new_sample;
                u8 index;
                samplePos %= kAudioBuffer * 2;
                new_sample = fileBuffer[samplePos];
                new_sample += fileBuffer[samplePos + 1] << 8;
                index = fileBuffer[samplePos + 2];
                _leftChannelDecoder.initPredictor((s16)new_sample, index);
                leftBuffer[(_sampleBufferPos + i) % kAudioBuffer] = (s16)new_sample;
                continue;
            }

            u8 byte = fileBuffer[samplePos % (kAudioBuffer * 2)];
            u8 nibble = byte;
            if (highBits)
                nibble >>= 4;
            nibble &= 0xF;

            s16 sample = _leftChannelDecoder.decodeSample(nibble);
            leftBuffer[(_sampleBufferPos + i) % kAudioBuffer] = sample;
        }

        DC_FlushRange(leftBuffer + _sampleBufferPos % kAudioBuffer, samples * 2);
    }

    void WAV::progress_ima_adpcm_stereo(u16 samples)
    {
        if (_leftBuffer == nullptr || _rightBuffer == nullptr)
            return;
        s16 *leftBuffer = reinterpret_cast<s16 *>(_leftBuffer.get());
        s16 *rightBuffer = reinterpret_cast<s16 *>(_rightBuffer.get());
        u8 *fileBuffer = _fileBuffer;
        for (int i = 0; i < samples * 2; i++)
        {
            u8 channelIdx = i % 2;
            bool highBits, readHeader;
            u32 samplePos;
            samplePos = ima_adpcm_get_sample_position(
                _sourceBufferPos + i / 2, channelIdx,
                highBits, readHeader);
            if (readHeader)
            {
                u16 new_sample;
                u8 index;
                samplePos %= kAudioBuffer * 2;
                new_sample = fileBuffer[samplePos];
                new_sample += fileBuffer[samplePos + 1] << 8;
                index = fileBuffer[samplePos + 2];
                if (channelIdx == 0)
                {
                    _leftChannelDecoder.initPredictor((s16)new_sample, index);
                    leftBuffer[(_sampleBufferPos + i / 2) % kAudioBuffer] = (s16)new_sample;
                }
                else
                {
                    _rightChannelDecoder.initPredictor((s16)new_sample, index);
                    rightBuffer[(_sampleBufferPos + i / 2) % kAudioBuffer] = (s16)new_sample;
                }
                continue;
            }

            u8 byte = fileBuffer[samplePos % (kAudioBuffer * 2)];
            u8 nibble = byte;
            if (highBits)
                nibble >>= 4;
            nibble &= 0xF;

            if (channelIdx == 0)
            {
                s16 sample = _leftChannelDecoder.decodeSample(nibble);
                leftBuffer[(_sampleBufferPos + i / 2) % kAudioBuffer] = sample;
            }
            else
            {
                s16 sample = _rightChannelDecoder.decodeSample(nibble);
                rightBuffer[(_sampleBufferPos + i / 2) % kAudioBuffer] = sample;
            }
        }

        DC_FlushRange(leftBuffer + _sampleBufferPos % kAudioBuffer, samples * 2);
        DC_FlushRange(rightBuffer + _sampleBufferPos % kAudioBuffer, samples * 2);
    }

    // Main sound function implementations
    void PlayBGM(BgmId bgm, bool loop)
    {
        StopBGM();
        cBGMusic.load(BGMS[bgm].path);
        cBGMusic.setVolume(80);
        cBGMusic.setLoops(loop ? -1 : 0);
        if (cBGMusic.getLoaded())
            cBGMusic.play();
    }

    void StopBGM()
    {
        cBGMusic.stop();
    }

    static std::list<WAV*> _sfxs;
    
    void LoadSFXs()
    {
        for (int sfx = 0; sfx < SFX_COUNT; sfx++)
        {            
            auto *sfxWav = new WAV();
            sfxWav->load(SFXS[sfx].path);
            sfxWav->setVolume(80);
            sfxWav->setLoops(0);
            _sfxs.push_back(sfxWav);
        }
    }

    void PlaySFX(SfxId sfx)
    {
        auto sfxWav = _sfxs.begin();
        std::advance(sfxWav, sfx);
        (*sfxWav)->play();
    }

    WAV cBGMusic;
}