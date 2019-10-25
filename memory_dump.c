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

	int opt;
	char *string = "a:s:xh";

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
			break;
		case 'h':
		default:
			printf("[%s] [opt]\n", argv[0]);
			printf("-a addr\n");
			printf("-s size\n");
			printf("-x print hex\n");
			printf("-h help");
			return 0;
			break;
		}
	}
	if(has_addr && has_size)
	{
		printf("addr = 0x%x, size = 0x%x is_print_hex=%s\n", addr, size, is_print_hex?"true":"false");
	}
	else
	{
		printf("[%s] [opt]\n", argv[0]);
		printf("-a addr\n");
		printf("-s size\n");
		printf("-x print hex\n");
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

	for(unsigned int i = 0; i < size; ++i)
	{
		if(is_print_hex)
		{
			printf("%02x ", map_base[i]);
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
