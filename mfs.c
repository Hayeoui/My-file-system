#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct data_info {
	char contents[128];
	struct data_info *data_p;
	struct data_info *left;
} data; // 포인터 byte는 따로 생각한다.
struct file_time {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int seconds;
}; 
typedef struct inode_info { 
	char *file_type;  
	struct file_time f_t;  
	int file_size;
	int db;
	int sib;
	int dib;
	data *d_p;
	data *s_ip;
	data *d_ip;
} inode;
typedef struct super_info {
	char inode_use[64]; // 8 * 64 = 512
	char datablock_use[128]; // 8 * 128 = 1024
} super;
inode *func_mymkfs(super *, inode *, struct tm *); // 0) mymkfs
void func_mymkdir(super *, inode *, char [], struct tm *, inode *); // 9) mymkdir
void func_myrmdir(super *, inode *, char [], inode *); // 10) myrmdir
void func_mytouch(super *, inode *, char [], struct tm *); // 13) mytouch
void func_mystate(super *); // 16 ) mystate
data *name_find(super *, inode *, char [], int *);
int main(void)
{
	struct tm *t;
	time_t now;
	now = time(NULL);
	t = localtime(&now);
	// 파일 생성 날짜

	unsigned short boot_block;
	super superblock = {{0}, {0}}; 
	inode inode_list[512] = {0};

	int c = 0;
	char words[100];
	int i, j, co_num, space = 0;
	char compare[20]; 
	static int check_mymkfs = 0;

	char *command[18] = {"mymkfs", "myls", "mycat", "myshowfile", "mypwd", "mycd", "mycp", "mycpto", "mycpfrom", "mymkdir", "myrmdir", "myrm", "mymv", "mytouch", "myshowinode", "myshowblock", "mystate", "mytree"};
	
	char *file_name = "myfs";   // mymkfs파일

	inode *now_d = &inode_list[0]
		;    // 현재 디렉터리 가리키는 포인터.


	while(1)
	{
		if(access(file_name, F_OK)!=0)
			printf("error : no myfs\n");

		co_num = 0;
		space = 0;
		for(i=0; words[i-1]!='\n'; ++i)
		{
			c = getchar();
			words[i] = c;
		}
		words[i] = '\0';

		for(i=0; words[i]!='\0'; ++i)
			if(words[i] == ' ')
				space++;

		if(space==0)
		{
			for(i=0; words[i]!='\n'; ++i)
				compare[i] = words[i];
			compare[i] = '\0';
		}
		else
		{
			for(i=0; words[i]!=' '; ++i)
				compare[i] = words[i];
			compare[i] = '\0';
		}

		if(space==0)
		{
			for(j=i; words[j]!='\0'; ++j)
				words[j-(i)] = words[j];
			words[j-(i)] = '\0';
		}
		else
		{
			for(j=i+1; words[j]!='\0'; ++j)
				words[j-(i+1)] = words[j];
			words[j-(i+1)] = '\0';
		}

		for(co_num=0; co_num<18; co_num++)
		{
			if(strcmp(compare, command[co_num])==0)	
			break;
		}


		// 명령어 함수 호출 부분 시작
		

		if(co_num == 0)
		{
			if(access(file_name, F_OK) == 0)
				printf("error : myfs exists\n");
			else
				now_d = func_mymkfs(&superblock, inode_list, t); // mymkfs실행 후 myfs가 없을 때 만듬.
		}
		else
		{
			if(access(file_name, F_OK) == 0)
			{
				if(co_num == 1)
				{
				}
				else if(co_num == 2)
				{
				}
				else if(co_num == 3)
				{
				}
				else if(co_num == 4)
				{
				}
				else if(co_num == 5)
				{
				}
				else if(co_num == 6)
				{
				}
				else if(co_num == 7)
				{
				}
				else if(co_num == 8)
				{
				}
				else if(co_num == 9)
				{
					func_mymkdir(&superblock, inode_list, words, t, now_d);
				}
				else if(co_num == 10)
				{
					func_myrmdir(&superblock, inode_list, words, now_d);
				}
				else if(co_num == 11)
				{
				}
				else if(co_num == 12)
				{
				}
				else if(co_num == 13)
				{
					func_mytouch(&superblock, inode_list, words, t);
				}
				else if(co_num == 14)
				{
				}
				else if(co_num == 15)
				{
				}
				else if(co_num == 16)
				{
					func_mystate(&superblock);
				}
				else if(co_num == 17)
				{
				}
				else // co_num == 18(cygwin 명령어 실행)
				{
					if(strcmp(compare, "byebye")==0)
						exit(0);
					else
					{
						for(i=0; compare[i]!='\0'; ++i)
							;
						if(words[0] == '\n')
							system(compare);
						else
						{
							compare[i] = ' ';
							for(j=i; words[i]!='\0'; ++j)
									compare[j+1] = words[j-i];
							compare[j+1] = '\0';
							system(compare);
						}
						
					}
				}
			}
			else
				printf("error : no myfs\n");
		}
	}

		
		return 0;
}
inode *func_mymkfs(super *sp, inode *ip, struct tm *t)
{
	system("touch myfs");
	// myfs파일에 부트블록, 슈퍼블록, 아이노드 리스트, 데이터블록 만들기
	
	time_t now;
	now = time(NULL);
	t = localtime(&now);


	ip[0].file_type = "directory";
	ip[0].f_t.year = 1900+t->tm_year;
	ip[0].f_t.month = 1+t->tm_mon;
	ip[0].f_t.day = t->tm_mday;
	ip[0].f_t.hour = t->tm_hour;
	ip[0].f_t.minute = t->tm_min;
	ip[0].f_t.seconds = t->tm_sec;
	ip[0].file_size = 0;
	ip[0].db = 1;
	ip[0].sib = 0;  // 디렉토리는 sib, dib 없음, 0
	ip[0].dib = 0;  // 0 = 없음.
	ip[0].d_p = (data *)malloc(sizeof(data));
	ip[0].d_p->data_p = NULL;
	ip[0].d_p->contents[0] = ' ';
	ip[0].d_p->contents[1] = '\0';
	ip[0].s_ip = NULL;
	ip[0].d_ip = NULL;
	ip[0].d_p->left = NULL; // 루트 디렉터리는 부모 디렉터리 없음.

	
	sp -> inode_use[0] = 1 << 0;
	sp -> datablock_use[0] = 1 << 0;

	return &ip[0];
}
void func_mymkdir(super *sp, inode *ip, char words[], struct tm *t, inode *now_d)
{
	int i, j, stop = 0, num = 0, d;
	int space = 0;
	int quotient;
	int bit[8];
	data *p;

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	for(i=0; words[i]!='\0'; ++i)
		if(words[i] == ' ')
			space++;

	if(space == 0)
	{
		if(words[0] == '\n')
		{
			printf("mymkdir : missing operand\n");
		}
		else
		{
			for(i=0; words[i]!='\0'; ++i)
				;
			words[i-1] = words[i];

			p = name_find(sp, ip, words, &num);

			if(p==now_d->d_p) // 디렉토리를 찾은 위치 p와 현재위치가 같으면 존재.
			{
				printf("error : [%s] directory exists\n", words);
			}
			else // 같은 이름이 다른파일에 존재하면 만들 수 있음. 또는 아예 없을 때.
			{
				// inode 비트열에서 0인 부분 찾기.
				for(i=0; i<64; ++i)
				{
					quotient = sp -> inode_use[i];
					for(j=0; j<8; ++j)
					{
						bit[j] = quotient % 2;
						quotient = quotient / 2;
					}
			
					for(j=0; j<8; ++j)
						if(bit[j] == 0)
						{
							stop = 1;
							break;
						}
			
					if(stop == 1)
						break;
			
				}
			
				// 0인 부분을 1로 바꿔줌.
				sp -> inode_use[i] = (sp -> inode_use[i]) ^ (1 << j);
			
				j = 8 * i + j;

				// datablock 비트열에서 0인 부분 찾기.
				for(i=0; i<128; ++i)
				{
					quotient = sp -> datablock_use[i];
					for(d=0; d<8; ++d)
					{
						bit[d] = quotient % 2;
						quotient = quotient / 2;
					}
					for(d=0; d<8; ++d)
						if(bit[d] == 0)
						{
							stop = 1;
							break;
						}
					if(stop == 1)
						break;
				}

				sp -> datablock_use[i] = (sp -> datablock_use[i]) ^ (1 << d);
				d = i * 8 + d;  // d가 데이터블록 번호.
				d = d + 1;
				
				// inode에 가서 정보를 넣어줌.
				ip[j].file_type = "directory";
				ip[j].f_t.year = 1900+t->tm_year;
				ip[j].f_t.month = 1+t->tm_mon;
				ip[j].f_t.day = t->tm_mday;
				ip[j].f_t.hour = t->tm_hour;
				ip[j].f_t.minute = t->tm_min;
				ip[j].f_t.seconds = t->tm_sec;
				ip[j].file_size = 0;
				ip[j].db = d; // data block 번호가 d번.
				ip[j].sib = 0; // mymkdir은 디렉토리 만드는 명령이므로 sib, dib 없음.
				ip[j].dib = 0; // 0 = 없음.
				ip[j].d_p = (data *)malloc(sizeof(data));
				ip[j].d_p -> data_p = NULL;
				ip[j].d_p -> left = NULL;
				ip[j].d_p -> contents[0] = ' ';
				ip[j].d_p -> contents[1] = '\0';
				ip[j].s_ip = NULL;
				ip[j].d_ip = NULL;
		
				// 그 전 파일/디렉터리의 끝을 찾아 만든 디렉터리와 연결.
				
				// 왼쪽 연결
				data *tmp = ip[0].d_p;
			
				while(tmp->data_p != NULL)
				{
					tmp = tmp->data_p;
				}
			
				tmp->data_p = ip[j].d_p;
	
				ip[j].d_p->left = tmp;


				// 오른쪽 연결
				// mymkdir로 만든 디렉터리 번호는 d
				
				int k = 0;

				for(k=0; k<512; ++k)
					if(ip[k].db == d + 1)  // 만든 디렉터리 다음 데이터블록.
						break;

				if(k == 512)    // 다음 데이터블록이 없음.
					ip[j].d_p->data_p = NULL;
				else
				{
					ip[j].d_p->data_p = ip[k].d_p;
					ip[k].d_p->left = ip[j].d_p;
				}

				k = 0;
				
				for(k=0; ; ++k)
				{
					if((now_d->d_p->contents[k]==' ')&&(now_d->d_p->contents[k+1]=='\0'))
						break;
					else
						;
				}
			
				if(k==0)
				{
					now_d->d_p->contents[k] = j / 100 + 48;
					j = j % 100;
					k = k + 1;
					now_d->d_p->contents[k] = j / 10 + 48;
					j = j % 10;
					k = k + 1;
					now_d->d_p->contents[k] = j / 1 + 48;
					j = j % 1;
					k = k + 1;
				}
				else
				{
					k = k + 1;
					now_d->d_p->contents[k] = j / 100 + 48;
					j = j % 100;
					k = k + 1;
					now_d->d_p->contents[k] = j / 10 + 48;
					j = j % 10;
					k = k + 1;
					now_d->d_p->contents[k] = j / 1 + 48;
					j = j % 1;
					k = k + 1;
				}
	
				// 디렉터리 이름 수 세기 i값이 글자 수
				for(i=0; words[i]!='\0'; ++i)
					;
	
				if(i<5)
				{
					for(i=0; words[i]!='\0'; ++i)
					{
						now_d->d_p->contents[k] = words[i];
						++k;
					}
					now_d->d_p->contents[k] = ' ';
					now_d->d_p->contents[k+1] = '\0';
	
				}
				else
				{
					for(i=0; i<4; ++i)
					{
						now_d->d_p->contents[k] = words[i];
						++k;
					}
					now_d->d_p->contents[k] = ' ';
					now_d->d_p->contents[k+1] = '\0';
	
				}
	
			}
	
		}
	}
	else
	{
		printf("error : input again\n");
	}
}
void func_mystate(super *sp)
{
	int i, j, quotient;
	int bit[8];
	int count_i = 0, count_d = 0;
	for(i=0; i<64; ++i)
	{
		quotient = sp -> inode_use[i];
		for(j=0; j<8; ++j)
		{
			bit[j] = quotient % 2;
			quotient = quotient / 2;
		}

		for(j=0; j<8; ++j)
			if(bit[j] == 0)
				++count_i;
	}

	for(i=0; i<128; ++i)
	{
		quotient = sp -> datablock_use[i];
		for(j=0; j<8; ++j)
		{
			bit[j] = quotient % 2;
			quotient = quotient / 2;
		}

		for(j=0; j<8; ++j)
			if(bit[j] == 0)
				++count_d;
	}

	printf("free inode : %d\n", count_i);
	printf("free data block : %d\n", count_d);
}
void func_mytouch(super *sp, inode *ip, char words[], struct tm * t)
{
	int i, j=0, k=0, quotient=0;
	int find = 0, in = 0;

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	data *p;

	for(i=0; words[i]!='\0'; ++i)
		;
	words[i-1] = words[i];

	p = name_find(sp, ip, words, &j);

	if(j != -2) // 디렉터리/파일 존재
	{
			in = 100 * (p->contents[j+1]-48) + 10 * (p->contents[j+2]-48) + (p->contents[j+3]-48);
			ip[in].f_t.year = 1900+t->tm_year;
			ip[in].f_t.month = 1+t->tm_mon;
			ip[in].f_t.day = t->tm_mday;
			ip[in].f_t.hour = t->tm_hour;
			ip[in].f_t.minute = t->tm_min;
			ip[in].f_t.seconds = t->tm_sec;
	}
	else // 디렉터리/파일 존재 x -> 크기가 0바이트인 파일 생성
	{
	}
}
void func_myrmdir(super *sp, inode *ip, char words[], inode *now_d)
{
	// o 데이터 블록 연결 끊고 좌 우 NULL 만들어 주고 메모리 반납
	// o 아이노드 가서 정보 삭제
	// 슈퍼블록에 비트열 0으로 바꿈 아이노드랑 데이터블록.
	// o 빈 디렉터리 아니면 못지움, 현재 디렉터리에 그 파일 있을 때만 지울수있음.
	// o 현재 디렉터리 contents 가서 아이노드랑 이름 지우고 당기기.


	int num, i, j;
	int in, dn;
	data *p;

	for(i=0; words[i]!='\0'; ++i)
		;
	words[i-1] = words[i];
	
	p = name_find(sp, ip, words, &num);

	if(p == now_d->d_p)
	{
		in = 100 * (p->contents[num+1] -48) + 10 * (p->contents[num+2]-48) + (p->contents[num+3]-48);

		if(strcmp("directory", ip[in].file_type)==0)
		{
			if(ip[in].d_p->contents[0] == ' '&&ip[in].d_p->contents[1]=='\0')
			{
				p->contents[num+1] = 0 + 48;
				p->contents[num+2] = 0 + 48;
				p->contents[num+3] = 0 + 48;

				for(i=0; i<128; ++i)
				{
					if(p->contents[i] == 48)
					{
						if(p->contents[i+1] == 48)
						{
							if(p->contents[i+2] == 48)
								break;
							else
								;
						}
						else
							;
					}
					else
						;
				}
				for(j=i; p->contents[j]!=' '; ++j)
					;
				for(j=j+1; p->contents[j]!='\0'; j++)
					p->contents[i++] = p->contents[j];
				p->contents[i] = '\0';


				if(ip[in].d_p->data_p!=NULL)
					ip[in].d_p->data_p->left = NULL;
				else
					;
				if(ip[in].d_p->left!=NULL)
					ip[in].d_p->left->data_p = NULL;
				else
					;

				free(ip[in].d_p);
				ip[in].d_p = NULL;
				
				dn = ip[in].db -1;  // dn = 데이터블록 인덱스값.
				// ip[in].db에 들어있는 값은 실제 데이터블록 번호.

				ip[in].file_type = "";
				ip[in].f_t.year = 0;
				ip[in].f_t.month = 0;
				ip[in].f_t.day = 0;
				ip[in].f_t.hour = 0;
				ip[in].f_t.minute = 0;
				ip[in].f_t.seconds = 0;
				ip[in].file_size = 0;
				ip[in].db = 0;
				ip[in].d_p = NULL;

				i = in / 8;
				j = in % 8;
				sp->inode_use[i] = (sp ->inode_use[i]) ^ (1 << j);

				i = dn / 8;
				j = dn % 8;
				sp->datablock_use[i] = (sp->datablock_use[i]) ^ (1 << j);


			}
			else
				printf("myrmdir : faled to remove '%s' : Direcotry not empty\n", words);

		}
		else
		{
			printf("myrmdir : failed to remove '%s' : Not a Directory\n", words);
		}
	}
	else
	{
		printf("myrmdir : falied to remove '%s' : No such file and directory\n", words);
	}
}
data *name_find(super *sp, inode *ip, char words[], int *num)
{
	data *p;
	int bit[8];
	int i = 0, j = 0, k = 0, quotient = 0;
	char dir_name[5];
	int find = 0;
	int use[512];
	int use_dir[512];
	for(i=0; i<64; ++i)
	{
		quotient = sp->inode_use[i];
		for(j=0; j<8; ++j)
		{
			bit[j] = quotient % 2;
			quotient = quotient / 2;
		}

		for(j=0; j<8; ++j)
			if(bit[j] == 1)
				use[k++] = 8 * i + j;

	}
	use[k] = -1;

	k=0;
	for(i=0; use[i]!=-1; ++i)
	{
		if(strcmp("directory", ip[use[i]].file_type)==0)
			use_dir[k++] = use[i];
		else
			;
	}
	use_dir[k] = -1;
	
	for(i=0; use_dir[i]!=-1; ++i)
	{
		p = ip[use_dir[i]].d_p;
		for(j=0; p->contents[j]!='\0'; ++j)
		{
			if(p->contents[j]==' ')
			{
				if(p->contents[j+1] == '\0')
					;
				else
				{
					for(k=0; p->contents[j+k+4]!=' '; ++k)
						dir_name[k] = p->contents[j+k+4];
					dir_name[k] = '\0';

					if(strcmp(words, dir_name)==0)
					{
						find = 1;
						break;
					}

				}
			}
		}
		if(p->contents[j] == '\0')
		{
			j = -1;
			if(p->contents[0] == ' ')
				;
			else
			{
				for(k=0; p->contents[j+k+4]!=' '; ++k)
					dir_name[k] = p->contents[j+k+4];
				dir_name[k] = '\0';
			}

			if(strcmp(dir_name, words)==0)
				find = 1;
		}

		if(find == 1)
		{
			*num = j;
			return p;
		}
	}
	*num = -2;
	return 0;
}

