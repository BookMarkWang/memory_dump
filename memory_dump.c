#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
	int fd;
	unsigned char* map_base;

	uint32_t addr = 0;
	uint32_t size = 0;
	bool is_print_hex = false;
	bool has_addr=false;
	bool has_size=false;
	uint8_t *data;
	uint32_t offset=0;
	uint8_t format=1;

	int opt;
	char *string = "a:s:x:h";

	while((opt = getopt(argc, argv, string))!= -1)
	{
		switch(opt)
		{
		case 'a':
			addr = strtoul(optarg, 0, 0);
			has_addr=true;
			break;
		case 's':
			size = strtoul(optarg, 0, 0);
			has_size=true;
			break;
		case 'x':
			is_print_hex = true;
			format = strtoul(optarg, 0, 0);
			if(format != 1 && format != 2 && format !=4)
			{
				format = 1;
			}
			break;
		case 'h':
		default:
			printf("[%s] [opt]\n", argv[0]);
			printf("-a addr\n");
			printf("-s size\n");
			printf("-x [format] : print hex with 1byte, 2bytes or four bytes, default is 1\n");
			printf("-h help");
			return 0;
			break;
		}
	}
	if(has_addr && has_size)
	{
		printf("addr = 0x%x, size = 0x%x is_print_hex=%s\n", addr, size, is_print_hex?"true":"false");
		offset = addr & 0xFFF;
		size = size + offset;
		addr &= 0xFFFFF000;
	}
	else
	{
		printf("[%s] [opt]\n", argv[0]);
		printf("-a addr\n");
		printf("-s size\n");
		printf("-x [format] : print hex with 1byte, 2bytes or four bytes, default is 1\n");
		printf("-o outfile : save logs to file\n");
		printf("-h help");
		return 0;
	}


	fd=open("/dev/mem",O_RDWR | O_SYNC);
	if(fd<0)
	{
		printf("open /dev/mem failed!");
		return -1;
	}

	map_base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if(map_base == 0)
	{
		printf("NULL pointer!\n");
		close(fd);
		return -1;
	}

	for(unsigned int i = offset; i < size; i += format)
	{
		if(is_print_hex)
		{
			switch(format)
			{
			case 1:
				{
					printf("%02x ", map_base[i]);
					break;
				}
			case 2:
				{
					uint16_t *tmp = (uint16_t*)&map_base[i];
					printf("%04x ", *tmp);
					break;
				}
			case 4:
				{
					uint32_t *tmp = (uint32_t*)&map_base[i];
					printf("%08x ", *tmp);
					break;
				}
			}
		}
		else
		{
			printf("%c", map_base[i]);
		}
	}
	printf("\n");

	munmap(map_base, size);

	close(fd);
	return 0;
}
