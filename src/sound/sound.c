#define AL_LIBTYPE_STATIC

#include <sound/sound.h>

#include <stdlib.h>
#include <string.h>

typedef struct {
    const unsigned char *data;
    size_t size;
    size_t offset;
} memory_buffer_t;

// Callback functions for reading OGG data from memory
size_t memory_read(void *ptr, size_t size, size_t nmemb, void *datasource) {
    memory_buffer_t *buffer = (memory_buffer_t *)datasource;
    size_t bytes_to_read = size * nmemb;
    if (buffer->offset + bytes_to_read > buffer->size) {
        bytes_to_read = buffer->size - buffer->offset;
    }
    memcpy(ptr, buffer->data + buffer->offset, bytes_to_read);
    buffer->offset += bytes_to_read;
    return bytes_to_read / size;
}

int memory_seek(void *datasource, ogg_int64_t offset, int whence) {
    memory_buffer_t *buffer = (memory_buffer_t *)datasource;
    size_t new_offset;
    switch (whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = buffer->offset + offset;
            break;
        case SEEK_END:
            new_offset = buffer->size + offset;
            break;
        default:
            return -1;
    }
    if (new_offset > buffer->size) {
        return -1;
    }
    buffer->offset = new_offset;
    return 0;
}

long memory_tell(void *datasource) {
    memory_buffer_t *buffer = (memory_buffer_t *)datasource;
    return buffer->offset;
}

// Decode OGG data from memory
int decode_ogg_memory(const unsigned char *data, size_t size, short **output, ALsizei *sample_rate, ALenum *format, ALsizei *buffer_size) {
    OggVorbis_File vf;
    memory_buffer_t buffer = {data, size, 0};

    ov_callbacks callbacks = {
        .read_func = memory_read,
        .seek_func = memory_seek,
        .close_func = NULL,
        .tell_func = memory_tell
    };

    if (ov_open_callbacks(&buffer, &vf, NULL, 0, callbacks) < 0) {
        fprintf(stderr, "Failed to open OGG data from memory.\n");
        return -1;
    }

    vorbis_info *info = ov_info(&vf, -1);
    *sample_rate = info->rate;
    int channels = info->channels;

    // Determine OpenAL format
    if (channels == 1) {
        *format = AL_FORMAT_MONO16;
    } else if (channels == 2) {
        *format = AL_FORMAT_STEREO16;
    } else {
        fprintf(stderr, "Unsupported channel count: %d\n", channels);
        ov_clear(&vf);
        return -1;
    }

    // Calculate total PCM size
    long samples = (long)ov_pcm_total(&vf, -1);
    *buffer_size = samples * channels * sizeof(short);

    // Allocate memory for PCM data
    *output = (short *)malloc(*buffer_size);
    if (!*output) {
        fprintf(stderr, "Failed to allocate memory for PCM data.\n");
        ov_clear(&vf);
        return -1;
    }

    // Read PCM data
    long total_read = 0;
    int bitstream;
    while (total_read < *buffer_size) {
        long ret = ov_read(&vf, (char *)(*output) + total_read, *buffer_size - total_read, 0, 2, 1, &bitstream);
        if (ret <= 0) break;
        total_read += ret;
    }

    ov_clear(&vf);
    return 0;
}

sound_t sound_load(const unsigned char *data, size_t size) {
    sound_t sound = {0};
    short *pcm_data = NULL;
    ALsizei sample_rate, buffer_size;
    ALenum format;

    if(decode_ogg_memory(data, size, &pcm_data, &sample_rate, &format, &buffer_size) < 0) {
        fprintf(stderr, "Failed to decode OGG data.\n");
        exit(EXIT_FAILURE);
    }

    // Generate OpenAL buffer and source
    alGenBuffers(1, &sound.buffer);
    alBufferData(sound.buffer, format, pcm_data, buffer_size, sample_rate);

    alGenSources(1, &sound.source);
    alSourcei(sound.source, AL_BUFFER, sound.buffer);

    sound.format = format;
    sound.sample_rate = sample_rate;

    free(pcm_data);
    return sound;
}

void sound_play(sound_t *sound, float volume, float pitch) {
    alSourcef(sound->source, AL_GAIN, volume);
    alSourcef(sound->source, AL_PITCH, pitch);
    alSourcePlay(sound->source);
}

void sound_stop(sound_t *sound) {
    alSourceStop(sound->source);
}

void sound_destroy(sound_t *sound) {
    alDeleteSources(1, &sound->source);
    alDeleteBuffers(1, &sound->buffer);
}