#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 4096 

typedef struct _SECURITY_HEADER
{
	uint32_t cbSize;
	unsigned char signature[12];
	uint32_t dwChunkSizeInKb;
	uint32_t dwAlgId;
	uint32_t dwCatalogSize;
	uint32_t dwHashTableSize;
} SECURITY_HEADER;

typedef struct _IMAGE_HEADER
{ 
	uint32_t cbSize;
	unsigned char signature[12];
	uint32_t manifestLength;
	uint32_t dwChunkSize;
} IMAGE_HEADER;

typedef struct _STORE_HEADER
{
	uint32_t dwUpdateType;
	uint16_t MajorVersion;
	uint16_t MinorVersion;
	uint16_t FullFlashMajorVersion;
	uint16_t FullFlashMinorVersion;	
	unsigned char szPlatformId[192];
	uint32_t dwBlockSizeInBytes;
	uint32_t dwWriteDescriptorCount;
	uint32_t dwWriteDescriptorLength;
	uint32_t dwValidateDescriptorCount;
	uint32_t dwValidateDescriptorLength;
	uint32_t dwInitialTableIndex;
	uint32_t dwInitialTableCount;
	uint32_t dwFlashOnlyTableIndex;
	uint32_t dwFlashOnlyTableCount;
	uint32_t dwFinalTableIndex;
	uint32_t dwFinalTableCount;
} STORE_HEADER;

typedef struct _DISK_LOCATION
{
	uint32_t dwDiskAccessMethod;
	uint32_t dwBlockIndex;
} DISK_LOCATION;

typedef struct _BLOCK_DATA_ENTRY
{
	uint32_t dwLocationCount;
	uint32_t dwBlockCount;
} BLOCK_DATA_ENTRY;

int main(int argc, char** argv) {
	SECURITY_HEADER security_header;
	IMAGE_HEADER image_header;
	STORE_HEADER store_header;
	BLOCK_DATA_ENTRY block_data_entry;
	DISK_LOCATION disk_location;

	FILE *fp;
	FILE *out;
	uint32_t i;
	uint32_t j;
	uint32_t k;
	uint32_t chunk_loops;
	char *bytes;
	char *offset;
	char *tmp;
	char *infile;
	char *outfile;
	uint32_t chunk_size;
	uint32_t padding;
	
	if(argc != 3) {
		printf("usage: ffu2dd infile outfile\n");
		return 0;
	}

	infile = argv[1];
	outfile = argv[2];

	fp = fopen(infile, "rb");
	fread(&security_header.cbSize, 1, 4, fp);
	fread(&security_header.signature, 1, 12, fp);
	fread(&security_header.dwChunkSizeInKb, 1, 4, fp);
	fread(&security_header.dwAlgId, 1, 4, fp);
	fread(&security_header.dwCatalogSize, 1, 4, fp);
	fread(&security_header.dwHashTableSize, 1, 4, fp);

	printf("---\nsecurity_header.cbSize: %d\n", security_header.cbSize);
	printf("signature: \"%s\"\n", security_header.signature);
	printf("dwChunkSizeInKb: %d\n", security_header.dwChunkSizeInKb);
	printf("dwAlgId: %d\n", security_header.dwAlgId);
	printf("dwCatalogSize: %d\n", security_header.dwCatalogSize);
	printf("dwHashTableSize: %d\n", security_header.dwHashTableSize);

	fseek(fp, security_header.dwCatalogSize, SEEK_CUR);
	fseek(fp, security_header.dwHashTableSize, SEEK_CUR);
	chunk_size = security_header.dwChunkSizeInKb * 1024;

	padding = chunk_size - (ftell(fp) % chunk_size);
	fseek(fp, padding, SEEK_CUR);
	printf("padding: %d\n", padding);	

	fread(&image_header.cbSize, 1, 4, fp);
	fread(&image_header.signature, 1, 12, fp);
	fread(&image_header.manifestLength, 1, 4, fp);
	fread(&image_header.dwChunkSize, 1, 4, fp);

	printf("---\nimage_header.cbSize: %d\n", image_header.cbSize);
	printf("signature: \"%s\"\n", image_header.signature);
	printf("manifestLength: %d\n", image_header.manifestLength);
	printf("dwChunkSize: %d\n", image_header.dwChunkSize);

	bytes = (char *)malloc(sizeof(char) * image_header.manifestLength);
	fread(bytes, 1, image_header.manifestLength, fp);
	printf("manifest: %s\n", bytes);
	free(bytes);	
	
	padding = chunk_size - (ftell(fp) % chunk_size);	
	fseek(fp, padding, SEEK_CUR);
	printf("padding: %d\n", padding);	

	fread(&store_header.dwUpdateType, 1, 4, fp);
	fread(&store_header.MajorVersion, 1, 2, fp);
	fread(&store_header.MinorVersion, 1, 2, fp);
	fread(&store_header.FullFlashMajorVersion, 1, 2, fp);
	fread(&store_header.FullFlashMinorVersion, 1, 2, fp);
	fread(&store_header.szPlatformId, 1, 192, fp);
	fread(&store_header.dwBlockSizeInBytes, 1, 4, fp);
	fread(&store_header.dwWriteDescriptorCount, 1, 4, fp);
	fread(&store_header.dwWriteDescriptorLength, 1, 4, fp);
	fread(&store_header.dwValidateDescriptorCount, 1, 4, fp);
	fread(&store_header.dwValidateDescriptorLength, 1, 4, fp);
	fread(&store_header.dwInitialTableIndex, 1, 4, fp);
	fread(&store_header.dwInitialTableCount, 1, 4, fp);
	fread(&store_header.dwFlashOnlyTableIndex, 1, 4, fp);
	fread(&store_header.dwFlashOnlyTableCount, 1, 4, fp);
	fread(&store_header.dwFinalTableIndex, 1, 4, fp);
	fread(&store_header.dwFinalTableCount, 1, 4, fp);
	
	printf("---\nstore_header.dwUpdateType: %u\n", store_header.dwUpdateType);
	printf("MajorVersion: %d, MinorVersion: %d\n", store_header.MajorVersion, store_header.MinorVersion);
	printf("FullFlashMajorVersion: %d, FullFlashMinorVersion: %d\n", store_header.FullFlashMajorVersion, store_header.FullFlashMinorVersion);
	printf("szPlatformId: \"%s\"\n", store_header.szPlatformId);
	printf("dwBlockSizeInBytes: %d\n", store_header.dwBlockSizeInBytes);
	printf("dwWriteDescriptorCount: %d\n", store_header.dwWriteDescriptorCount);
	printf("dwWriteDescriptorLength: %d\n", store_header.dwWriteDescriptorLength);
	printf("dwValidateDescriptorCount: %d\n", store_header.dwValidateDescriptorCount);
	printf("dwValidateDescriptorLength: %d\n", store_header.dwValidateDescriptorLength);
	printf("dwInitialTableIndex: %d\n", store_header.dwInitialTableIndex);
	printf("dwInitialTableCount: %d\n", store_header.dwInitialTableCount);
	printf("dwFlashOnlyTableIndex: %d\n", store_header.dwFlashOnlyTableIndex);
	printf("dwFlashOnlyTableCount: %d\n", store_header.dwFlashOnlyTableCount);
	printf("dwFinalTableIndex: %d\n", store_header.dwFinalTableIndex);
	printf("dwFinalTableCount: %d\n", store_header.dwFinalTableCount); 

	bytes = (char *)malloc(sizeof(char) * store_header.dwWriteDescriptorLength);
	fread(bytes, 1, store_header.dwWriteDescriptorLength, fp);
	offset = bytes;

	padding = chunk_size - (ftell(fp) % chunk_size);	
	fseek(fp, padding, SEEK_CUR);
	printf("padding: %d\n", padding);	
	printf("ftell: %d\n", ftell(fp));
	
	out = fopen(outfile, "wb");	
	
	tmp = (char*)malloc(sizeof(char) * CHUNK_SIZE);
	chunk_loops = store_header.dwBlockSizeInBytes / CHUNK_SIZE;
	
	for(i = 0; i < store_header.dwWriteDescriptorCount; i++) {
		memcpy(&block_data_entry.dwLocationCount, offset, sizeof(uint32_t)); 
		offset += sizeof(uint32_t);
		memcpy(&block_data_entry.dwBlockCount, offset, sizeof(uint32_t)); 
		offset += sizeof(uint32_t);

		if(block_data_entry.dwLocationCount > 1) {
			printf("---\ndwLocationCount: %d\n", block_data_entry.dwLocationCount);
		}

		if(block_data_entry.dwBlockCount > 1) {
			printf("dwBlockCount: %d\n", block_data_entry.dwBlockCount);
		}

		for(j = 0; j < block_data_entry.dwLocationCount; j++) {
			memcpy(&disk_location.dwDiskAccessMethod, offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(&disk_location.dwBlockIndex, offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			if(disk_location.dwDiskAccessMethod != 0) {
				printf("%d=%d\n", disk_location.dwDiskAccessMethod, disk_location.dwBlockIndex);
			}
				
			fseek(out, (uint64_t)store_header.dwBlockSizeInBytes * (uint64_t)disk_location.dwBlockIndex, SEEK_SET);	
			for(k = 0; k < chunk_loops; k++) { 
				fread(tmp, sizeof(char), CHUNK_SIZE, fp);
				fwrite(tmp, sizeof(char), CHUNK_SIZE, out);
			}
		}	
	}

	free(bytes);
	fclose(out);
	fclose(fp);	
	return 0;
}
