#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>

#include "globals.h"

#include "loadUtilities.h"

// Constants used to load DDS textures
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

namespace HillEngine
{

	GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath)
	{
		GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the code from the GLSL files
		std::string vertexShaderCode;
		std::ifstream vertexShaderStream(vertexFilePath, std::ios::in);
		if (vertexShaderStream.is_open())
		{
			std::string line = "";
			while (getline(vertexShaderStream, line))
				vertexShaderCode += "\n" + line;
			vertexShaderStream.close();
		}

		std::string fragmentShaderCode;
		std::ifstream fragmentShaderStream(fragmentFilePath, std::ios::in);
		if (fragmentShaderStream.is_open())
		{
			std::string line = "";
			while (getline(fragmentShaderStream, line))
				fragmentShaderCode += "\n" + line;
			fragmentShaderStream.close();
		}

		GLint result = GL_FALSE;
		int infoLogLength;

		// Compile vertex shader
		printf("Compiling shader : %s\n", vertexFilePath);
		char const* vertexSourcePointer = vertexShaderCode.c_str();
		glShaderSource(vertexShaderId, 1, &vertexSourcePointer, NULL);
		glCompileShader(vertexShaderId);

		// Check vertex shader
		glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
		glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> vertexShaderErrorMessage(max(infoLogLength, (int)1));
		glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &vertexShaderErrorMessage[0]);

		// Compile fragment shader
		printf("Compiling shader : %s\n", fragmentFilePath);
		char const* fragmentSourcePointer = fragmentShaderCode.c_str();
		glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, NULL);
		glCompileShader(fragmentShaderId);

		// Check fragment shader
		glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> fragmentShaderErrorMessage(max(infoLogLength, (int)1));
		glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &fragmentShaderErrorMessage[0]);

		// Link the program
		fprintf(stdout, "Linking program\n");
		GLuint programId = glCreateProgram();
		glAttachShader(programId, vertexShaderId);
		glAttachShader(programId, fragmentShaderId);
		glLinkProgram(programId);

		// Check the program
		glGetProgramiv(programId, GL_LINK_STATUS, &result);
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> programErrorMessage(max(infoLogLength, (int)1));
		glGetProgramInfoLog(programId, infoLogLength, NULL, &programErrorMessage[0]);
		fprintf(stdout, "%s\n", &programErrorMessage[0]);

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);

		return programId;
	}

	GLuint loadBMP(const char* filePath)
	{
		unsigned char header[54];
		unsigned int dataPos;
		unsigned int width, height;
		unsigned int imageSize;

		unsigned char* data;

		// Read the bitmap headers
		FILE* file = fopen(filePath, "rb");
		if (!file)
		{
			printf("Image could not be opened\n");
			return 0;
		}

		if (fread(header, 1, 54, file) != 54)
		{
			printf("Invalid BMP file\n");
			return 0;
		}

		if (header[0] != 'B' || header[1] != 'M')
		{
			printf("Invalid BMP file\n");
			return 0;
		}

		// Load metadata
		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		if (imageSize == 0) imageSize = width * height * 3;
		if (dataPos == 0) dataPos = 54;

		// Load data from the file
		data = new unsigned char[imageSize];

		fread(data, 1, imageSize, file);

		fclose(file);

		// Convert raw image data into openGL texture
		GLuint textureId;
		glGenTextures(1, &textureId);

		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		return textureId;
	}

	GLuint loadDDS(const char* filePath)
	{
		unsigned char header[124];

		FILE *fp;

		fp = fopen(filePath, "rb");
		if (fp == NULL)
			return 0;

		// Confirm the file is a DDS file
		char filecode[4];
		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0)
		{
			fclose(fp);
			return 0;
		}

		// Read the files header info
		fread(&header, 124, 1, fp);

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);

		unsigned char* buffer;
		unsigned int bufSize;
		bufSize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = (unsigned char*)malloc(bufSize * sizeof(unsigned char));
		fread(buffer, 1, bufSize, fp);
		fclose(fp);

		// Check DDS version
		unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
		unsigned int format;
		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
		}

		// Pass data to openGL to generate texture
		GLuint textureId;
		glGenTextures(1, &textureId);

		glBindTexture(GL_TEXTURE_2D, textureId);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;

		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) *blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;
		}
		free(buffer);

		return textureId;
	}

	bool loadOBJ(
		const char* path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals)
	{
		// Raw OBJ data
		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;

		FILE* file = fopen(path, "r");
		if (file == NULL)
		{
			printf("Could not open file\n");
			return false;
		}

		while (1)
		{
			char lineHeader[128];

			// Get the OBJ command
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0)
			{
				// Add a vertex
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				// Add a uv
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				// Add a normal
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				// Add a face, uses data from previous commands
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9)
				{
					printf("Simple parser is incompetent - meshes must contain normal and uv data and only triangular faces\n");
					return false;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		// Puts necessary data into output arrays
		for (unsigned int i = 0; i < vertexIndices.size(); ++i)
		{
			unsigned int vertexIndex = vertexIndices[i];
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			out_vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < uvIndices.size(); ++i)
		{
			unsigned int uvIndex = uvIndices[i];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			out_uvs.push_back(uv);
		}
		for (unsigned int i = 0; i < normalIndices.size(); ++i)
		{
			unsigned int normalIndex = normalIndices[i];
			glm::vec3 normal = temp_normals[normalIndex - 1];
			out_normals.push_back(normal);
		}

		return true;
	}

	// Helper structs for loading WAVs
	struct RIFF_Header
	{
		char chunkID[4];
		long chunkSize;
		char format[4];
	};

	struct WAVE_Format
	{
		char subChunkID[4];
		long subChunkSize;
		short audioFormat;
		short numChannels;
		long sampleRate;
		long byteRate;
		short blockAlign;
		short bitsPerSample;
	};

	struct WAVE_Data
	{
		char subChunkID[4];
		long subChunk2Size;
	};

	bool loadWAVFile(const char* filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format)
	{
		FILE* soundFile = NULL;
		WAVE_Format waveFormat;
		RIFF_Header riffHeader;
		WAVE_Data waveData;
		unsigned char* data;

		try
		{
			soundFile = fopen(filename, "rb");
			if (!soundFile)
				throw("Could not open file");

			// Read the first chunk into the struct
			fread(&riffHeader, sizeof(RIFF_Header), 1, soundFile);

			// Check for the RIFF and WAVE tags in memory
			if (riffHeader.chunkID[0] != 'R' ||
				riffHeader.chunkID[1] != 'I' ||
				riffHeader.chunkID[2] != 'F' ||
				riffHeader.chunkID[3] != 'F' ||
				riffHeader.format[0] != 'W' ||
				riffHeader.format[1] != 'A' ||
				riffHeader.format[2] != 'V' ||
				riffHeader.format[3] != 'E')
				throw("Invalid RIFF or WAVE header");

			// Read the 2nd chunk
			fread(&waveFormat, sizeof(WAVE_Format), 1, soundFile);

			// Check for the fmt tag
			if (waveFormat.subChunkID[0] != 'f' ||
				waveFormat.subChunkID[1] != 'm' ||
				waveFormat.subChunkID[2] != 't' ||
				waveFormat.subChunkID[3] != ' ')
				throw("Invalid wave format");

			// Check for extra parameters
			if (waveFormat.subChunkSize > 16)
				fseek(soundFile, sizeof(short), SEEK_CUR);

			// Read om the last byte of data before the sound file
			fread(&waveData, sizeof(WAVE_Data), 1, soundFile);
			
			// Check for data tag
			if (waveData.subChunkID[0] != 'd' ||
				waveData.subChunkID[1] != 'a' ||
				waveData.subChunkID[2] != 't' ||
				waveData.subChunkID[3] != 'a')
				throw("Invalid data header");

			data = new unsigned char[waveData.subChunk2Size];

			// Read in the sound data
			if (!fread(data, waveData.subChunk2Size, 1, soundFile))
				throw ("Error loading WAVE");

			// Set the variables passed to the function
			*size = waveData.subChunk2Size;
			*frequency = waveFormat.sampleRate;

			// Work out the format from the number of channels and bits per sample
			if (waveFormat.numChannels == 1)
			{
				if (waveFormat.bitsPerSample == 8)
					*format = AL_FORMAT_MONO8;
				else if (waveFormat.bitsPerSample == 16)
					*format = AL_FORMAT_MONO16;
			}
			else if (waveFormat.numChannels == 2)
			{
				if (waveFormat.bitsPerSample == 8)
					*format = AL_FORMAT_STEREO8;
				else if (waveFormat.bitsPerSample == 16)
					*format = AL_FORMAT_STEREO16;
			}

			// Create openAL buffer
			alGenBuffers(1, buffer);
			if (alGetError() != AL_NO_ERROR)
				throw("Error creating buffer");

			// Put wave data into buffer
			alBufferData(*buffer, *format, (void*)data, *size, *frequency);
			if (alGetError() != AL_NO_ERROR)
				throw("Error storing data in buffer");

			// Clean up and return true
			fclose(soundFile);
			return true;
		}
		catch (const char* error)
		{
			printf("%s : trying to load %s\n", error, filename);

			// Still need to clean up
			if (soundFile != NULL)
				fclose(soundFile);

			return false;
		}
	}

} // namespace HillEngine