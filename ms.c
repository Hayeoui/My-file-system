#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
union file_contents {
	char dc[24][7];
	char fc[128];
	short ind[102];
};
typedef struct data_info {
	union file_contents con;
	struct data_info *next;
	struct data_info *down;
	struct data_info *link;
	int parent;
	int d_num;
	char type;
} data; // 포인터 byte는 따로 생각한다.
struct file_time {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int seconds  ;
}; 
typedef struct inode_info { 
	char *file_type;  
	struct file_time f_t;  
	int file_size;
	int db;
	int sib;
	int dib;
} inode;
typedef struct super_info {
	unsigned char inode_use[64]; // 8 * 64 = 512
	unsigned char datablock_use[128]; // 8 * 128 = 1024
} super;
data *dblock[1024] = {NULL};
int update_mymkfs(super *, inode *, inode **);
int func_mymkfs(super *, inode *, struct tm *, inode **); // 0) mymkfs
void func_mystate(super *); // 16 ) mystate
void func_mycpfrom(super *, inode *, char [], struct tm *, int, inode *); // 8)mycpfrom
void func_mymkdir(super *, inode *, char [], struct tm *, int, inode *); // 9) mymkdir
void func_myrmdir(super *, char [], inode *, inode *);  // 10) myrmdir
void func_mytouch(char [], super *, inode *, inode *, struct tm *, int); // 13) mytouch
void func_myshowinode(char [], inode *);  // 14) myshowinode
void func_myshowblock(char [], inode *); // 15) myshowblock
int space_inode(super *);
int space_data(super *);
void bit_inode(super *, int);
void bit_data(super *, int);
int name_find(inode *, char [], inode *);
void name_write(int, inode *, super *, char []);
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
	int i, j, co_num, space = 0, check = 0;
	int find = 0;
	char compare[20]; 

	char *command[18] = {"mymkfs", "myls", "mycat", "myshowfile", "mypwd", "mycd", "mycp", "mycpto", "mycpfrom", "mymkdir", "myrmdir", "myrm", "mymv", "mytouch", "myshowinode", "myshowblock", "mystate", "mytree"};
	
	char *file_name = "myfs";   // mymkfs파일
	int mymkfs_check = 0;

	inode *now_d = &inode_list[0];
	int now_d_num;
		    // 현재 디렉터리 가리키는 포인터.
	
	data *tmp = NULL;


	while(1)
	{
		if(access(file_name, F_OK)!=0)
		{
			printf("error : no myfs\n");
			mymkfs_check = 1;
		}
		else
		{
			if(mymkfs_check == 0)
				now_d_num = update_mymkfs(&superblock, inode_list, &now_d); // 있으면 불러오기.
			mymkfs_check = 1;
		}

		co_num = 0;
		space = 0;
		check = 0;
		for(i=0; words[i-1]!='\n'; ++i)
		{
			c = getchar();
			words[i] = c;
		}
		words[i-1] = '\0';


		for(i=0; words[i]!='\0'; ++i)
			if(words[i] == ' ')
				space++;

		if(space==0)
		{
			for(i=0; words[i]!='\0'; ++i)
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
			words[0] = '\0';
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
				now_d_num = func_mymkfs(&superblock, inode_list, t, &now_d); // mymkfs실행 후 myfs가 없을 때 만듬.
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
					func_mycpfrom(&superblock, inode_list, words, t, now_d_num, now_d);
				}
				else if(co_num == 9)
				{
					for(i=0; words[i]!='\0'; ++i)
						;
					if(i>4)
						words[4] = '\0';
					else
						;

					find = name_find(inode_list, words, now_d);
					if(find == 0)
					{
						if(words[0] == '\0')
							printf("mymkdir : missing operand\n");
						else
						{
							if(words[0] == ' ')
								printf("mymkdir : try input again\n");
							else
							{
								for(i=0; words[i]!='\0'; ++i)
									if(words[i]==' ')
									{
										check = 1;
										printf("mymkdir : too many operand\n");
										break;
									}

								if(check!=1)
									func_mymkdir(&superblock, inode_list, words, t, now_d_num, now_d);
							}
						}
						tmp = dblock[0];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								printf("%d) ", i);
								printf("%s\n", &(tmp->con.dc[i][3]));
							}
							tmp = tmp -> link;
						}
					}
					else  // 있으면 아이노드 값 리턴.
					{
						printf("mymkdir : '%s' 디렉토리를 만들 수 없습니다 : File exists\n", words);
					}
				}
				else if(co_num == 10)
				{
					for(i=0; words[i]!='\0'; ++i)
						;
					if(i>4)
						words[4] = '\0';
					else
						;
					
					func_myrmdir(&superblock, words, inode_list, now_d);
						tmp = dblock[0];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								printf("%d) ", i);
								printf("%s\n", &(tmp->con.dc[i][3]));
							}
							tmp = tmp -> link;
						}
				}
				else if(co_num == 11)
				{
				}
				else if(co_num == 12)
				{
				}
				else if(co_num == 13)
				{
					func_mytouch(words, &superblock, inode_list, now_d, t, now_d_num);
				}
				else if(co_num == 14)
				{
					func_myshowinode(words, inode_list);
				}
				else if(co_num == 15)
				{
					func_myshowblock(words, inode_list);
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
int update_mymkfs(super *sp, inode *ip, inode **now_dp)
{
	// 이미 myfs가 있어서 myfs에 들어있는 내용 불러오기.
	// 불러온 후 파일시스템 실행해도 기본 위치는 / 디렉토리.
	printf("updating.....\n");

	*now_dp = &ip[0];
	
	return 0;
}
int func_mymkfs(super *sp, inode *ip, struct tm *t, inode **now_dp)
{
	// 파일시스템 실행 후 myfs가 없을때 생성.
	system("touch myfs");
	
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
	ip[0].db = 0;  // 0번 데이터 블록
	ip[0].sib = -1;  
	ip[0].dib = -1;  // -1 = 없음.
	dblock[0] = (data *)malloc(sizeof(data));
	dblock[0]->con.dc[0][0] = '\0';
	dblock[0]->next = NULL;
	dblock[0]->down = NULL;
	dblock[0]->link = NULL;
	dblock[0]->d_num = 0;
	dblock[0]->parent = -1; // -1 = 없음.
	dblock[0]->type = 'd';
	
	sp -> inode_use[0] = 1 << 0;
	sp -> datablock_use[0] = 1 << 0;

	*now_dp = &ip[0];

	return 0;
}
void func_mytouch(char words[], super *sp, inode *ip, inode *now_d, struct tm *t, int now_d_num)
{
	int i, in, dn;
	data *tmp;

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	if(words[0] == '\0')
		printf("mytouch : missing operand\n");
	else
	{
		if(words[0] == ' ')
			printf("Try input again\n");
		else
		{
			for(i=0; words[i]!='\0'; ++i)
				if(words[i] == ' ')
					break;

			if(words[i]!='\0')
				printf("Try input again\n");
			else
			{
				in = name_find(ip, words, now_d);

				if(in == 0)
				{
					in = space_inode(sp);
					bit_inode(sp, in);

					ip[in].file_type = "regular file";
					ip[in].f_t.year = 1900+t->tm_year;
					ip[in].f_t.month = 1+t->tm_mon;
					ip[in].f_t.day = t->tm_mday;
					ip[in].f_t.hour = t->tm_hour;
					ip[in].f_t.minute = t->tm_min;
					ip[in].f_t.seconds = t->tm_sec;
					ip[in].file_size = 0;
					ip[in].sib = -1;
					ip[in].dib = -1;

					dn = space_data(sp);
					bit_data(sp, dn);

					ip[in].db = dn;

					dblock[dn] = (data *)malloc(sizeof(data));
					dblock[dn]->con.fc[0] = '\0';
					dblock[dn]->next = NULL;
					dblock[dn]->link = NULL;
					dblock[dn]->down = NULL;
					dblock[dn]->parent = now_d_num;
					dblock[dn]->d_num = dn;
					
					if(dblock[now_d->db]->con.dc[0][0]=='\0')
						dblock[now_d->db]->down = dblock[dn];
					else
					{
						tmp = dblock[now_d->db]->down;
						while(tmp->next!=NULL)
						{
							tmp = tmp->next;
						}
						tmp->next = dblock[dn];
					}

					name_write(in, now_d, sp, words);
				}
				else
				{
					ip[in].f_t.year = 1900+t->tm_year; 
					ip[in].f_t.month = 1+t->tm_mon;
					ip[in].f_t.day = t->tm_mday;
					ip[in].f_t.hour = t->tm_hour;
					ip[in].f_t.minute = t->tm_min;
					ip[in].f_t.seconds = t->tm_sec;
				}
			}
		}
	}
}
void func_mycpfrom(super *sp, inode *ip, char words[], struct tm *t, int now_d_num, inode *now_d)
{
	char f1[20], f2[5];
	int i, j, check, byte=0, c;
	int scount = 0, dcount = 0, in, dn;
	int s_dn, count;
	int num=0;
	data *tmp;
	FILE *ofp;

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	if(words[0] == ' ')
		printf("Try input again\n");
	else
	{
		for(i=0; words[i]!=' '; ++i)
			f1[i] = words[i];
		f1[i] = '\0';
	
		if(f1[0] == '\0')
			printf("mycpfrom : missing operand\n");
		else
		{
			for(i=0; words[i]!='\0'; ++i)
				if(words[i]==' ')
					break;

			if(words[i]=='\0')
				printf("mycpfrom : missing operand\n");
			else
			{
	
				if(words[i+1]==' ')
					printf("Try input again\n");
				else if(words[i+1]=='\0')
					printf("mycpfrom : missing operand\n");
				else
				{
					for(j=0, i=i+1; j<4&&words[i]!='\0'; ++i)
					{
						f2[j] = words[i];
						++j;
					}
					f2[j] = '\0';

					if(access(f1, F_OK)!=0)
						printf("'%s' not exists in Cygwin\n", f1);
					else
					{
						check = name_find(ip, f2, now_d); 
						if(check != 0)
							printf("'%s' is already exists in My-file-system\n", f2);
						else
						{
							ofp = fopen(f1, "rb");

							while(fscanf(ofp, "%c", &c)!=EOF)
							{
								byte++;
							}

							fclose(ofp);

							if((byte % 128) == 0)
								num = byte / 128;
							else
								num = (byte / 128) + 1;

							// 개수에 따라서 direct, single, double 만들기
							if(num == 0) // 파일이 0바이트여서 데이터블록 할당 안함.
							{
								func_mytouch(f2, sp, ip, now_d, t, now_d_num);
							}
							else if(num == 1)
							{
								in = space_inode(sp);
								bit_inode(sp, in);

								ip[in].file_type = "regular file";
								ip[in].f_t.year = 1900+t->tm_year;
								ip[in].f_t.month = 1+t->tm_mon;
								ip[in].f_t.day = t->tm_mday;
								ip[in].f_t.hour = t->tm_hour;
								ip[in].f_t.minute = t->tm_min;
								ip[in].f_t.seconds = t->tm_sec;
								ip[in].file_size = byte;

								dn = space_data(sp);
								bit_data(sp, dn);

								ip[in].db = dn;
								ip[in].sib = -1;
								ip[in].dib = -1;

								dblock[dn] = (data *)malloc(sizeof(data));
								dblock[dn]->link = NULL;
								dblock[dn]->next = NULL;
								dblock[dn]->down = NULL;
								dblock[dn]->parent = now_d_num;
								dblock[dn]->d_num = dn;
								dblock[dn]->type = 'f';
								// 데이터블록 1개에 내용 넣기.
								
								ofp = fopen(f1, "rb");
								for(i=0; i<byte; ++i)
									fscanf(ofp, "%c", &(dblock[dn]->con.fc[i]));

								fclose(ofp);

								// 형제 or 부모 찾아서 연결
								if(dblock[now_d->db]->con.dc[0][0]=='\0')
									dblock[now_d->db]->down = dblock[dn];
								else
								{
									tmp = dblock[now_d->db]->down;
									while(tmp->next!=NULL)
									{
										tmp = tmp->next;
									}
									tmp->next = dblock[dn];
								}
								name_write(in, now_d, sp, f2);
							}
							else if((num>1)&&(num<104))
							{
								in = space_inode(sp);
								bit_inode(sp, in);

								ip[in].file_type = "regular file";
								ip[in].f_t.year = 1900+t->tm_year;
								ip[in].f_t.month = 1+t->tm_mon;
								ip[in].f_t.day = t->tm_mday;
								ip[in].f_t.hour = t->tm_hour;
								ip[in].f_t.minute = t->tm_min;
								ip[in].f_t.seconds = t->tm_sec;
								ip[in].file_size = byte;

								// 첫번째 데이터블록 생성.
								dn = space_data(sp);
								bit_data(sp, dn);

								ip[in].db = dn;
								ip[in].sib = -1;
								ip[in].dib = -1;

								dblock[dn] = (data *)malloc(sizeof(data));
								dblock[dn]->next = NULL;
								dblock[dn]->link = NULL;
								dblock[dn]->down = NULL;
								dblock[dn]->parent = now_d_num;
								dblock[dn]->d_num = dn;
								dblock[dn]->type = 'f';
								//128바이트까지 첫번째 데이터블록에 내용 넣기.
								ofp = fopen(f1, "rb");

								for(i=0; i<128; ++i)
									fscanf(ofp, "%c", &(dblock[dn]->con.fc[i]));

								//부모 or 형제 디렉터리와 연결
								if(dblock[now_d->db]->con.dc[0][0]=='\0')
									dblock[now_d->db]->down = dblock[dn];
								else
								{
									tmp = dblock[now_d->db]->down;
									while(tmp->next!=NULL)
									{
										tmp = tmp->next;
									}
									tmp -> next = dblock[dn];
								}

								// single indirect 데이터블록 만들기
								dn = space_data(sp);
								bit_data(sp, dn);

								dblock[dn] = (data *)malloc(sizeof(data));
								dblock[dn]->con.ind[0] = 0;
								dblock[dn]->next = NULL;
								dblock[dn]->link = NULL;
								dblock[dn]->down = NULL;
								dblock[dn]->parent = -1;
								dblock[dn]->d_num = dn;
								dblock[dn]->type = 'i';
								ip[in].sib = dn;

								count = 2;
								while(count<num+1)
								{
									dn = space_data(sp);
									bit_data(sp, dn);

									dblock[dn] = (data *)malloc(sizeof(data));
									dblock[dn]->next = NULL;
									dblock[dn]->link = NULL;
									dblock[dn]->down = NULL;
									dblock[dn]->parent = -1;
									dblock[dn]->d_num = dn;
									dblock[dn]->type = 'f';
									//128바이트 내용 넣기
									i=0;
									while(fscanf(ofp, "%c", &(dblock[dn]->con.fc[i]))!=EOF&&i<128)
									{
										i++;
									}

									fclose(ofp);
									tmp = dblock[ip[in].db];
									while(tmp->link!=NULL)
									{
										tmp = tmp->link;
									}
									tmp->link = dblock[dn];

									dblock[ip[in].sib]->con.ind[count-2] = dn;
									if(count != 103)
										dblock[ip[in].sib]->con.ind[count-1] = 0;

									count++;
								}
								name_write(in, now_d, sp, f2);

							}
							else
							{
								in = space_inode(sp);
								bit_inode(sp, in);

								ip[in].file_type = "regular file";
								ip[in].f_t.year = 1900+t->tm_year;
								ip[in].f_t.month = 1+t->tm_mon;
								ip[in].f_t.day = t->tm_mday;
								ip[in].f_t.hour = t->tm_hour;
								ip[in].f_t.minute = t->tm_min;
								ip[in].f_t.seconds = t->tm_sec;
								ip[in].file_size = byte;

								// 첫번쨰 데이터 블록 생성.
								dn = space_data(sp);
								bit_data(sp, dn);

								ip[in].db = dn;
								ip[in].sib = -1;
								ip[in].dib = -1;

								dblock[dn] = (data *)malloc(sizeof(data));
								dblock[dn]->next = NULL;
								dblock[dn]->link = NULL;
								dblock[dn]->down = NULL;
								dblock[dn]->parent = now_d_num;
								dblock[dn]->d_num = dn;
								dblock[dn]->type = 'f';
								//128바이트까지 첫번째 데이터블록에 내용 넣기.
								ofp = fopen(f1, "rb");
								for(i=0; i<128; ++i)
									fscanf(ofp, "%c", &(dblock[dn]->con.fc[i]));
								//부모 or 형제 디렉터리와 연결
								if(dblock[now_d->db]->con.dc[0][0]=='\0')
									dblock[now_d->db]->down = dblock[dn];
								else
								{
									tmp = dblock[now_d->db]->down;
									while(tmp->next!=NULL)
									{
										tmp = tmp->next;
									}
									tmp->next = dblock[dn];
								}

								// single indirect 데이터블록 만들기
								dn = space_data(sp);
								bit_data(sp, dn);

								dblock[dn] = (data *)malloc(sizeof(data));
								dblock[dn]->con.ind[0] = 0;
								dblock[dn]->next = NULL;
								dblock[dn]->link = NULL;
								dblock[dn]->down = NULL;
								dblock[dn]->parent = -1;
								dblock[dn]->d_num = dn;
								dblock[dn]->type = 'i';
								ip[in].sib = dn;

								count = 2;
								while(count<104)
								{
									dn = space_data(sp);
									bit_data(sp, dn);

									dblock[dn] = (data *)malloc(sizeof(data));
									dblock[dn]->next = NULL;
									dblock[dn]->link = NULL;
									dblock[dn]->down = NULL;
									dblock[dn]->parent = -1;
									dblock[dn]->d_num = dn;
									dblock[dn]->type = 'f';
									//128바이트 내용 넣기
									for(i=0; i<128; ++i)
										fscanf(ofp, "%c", &(dblock[dn]->con.fc[i]));

									tmp = dblock[ip[in].db];
									while(tmp->link!=NULL)
									{
										tmp = tmp->link;
									}
									tmp->link = dblock[dn];

									dblock[ip[in].sib]->con.ind[count-2] = dn;
									if(count != 103)
										dblock[ip[in].sib]->con.ind[count-1] = 0;

									count++;
								}

								dn = space_data(sp);
								bit_data(sp, dn);

								dblock[dn] = (data *)malloc(sizeof(data));
								dblock[dn]->link = NULL;
								dblock[dn]->next = NULL;
								dblock[dn]->down = NULL;
								dblock[dn]->type = 'i';
								ip[in].dib = dn;
								dblock[dn]->con.ind[0] = 0;

								while(count < num+1)
								{
									if(count % 102 == 2)
									{
										dn = space_data(sp);
										bit_data(sp, dn);
										
										dblock[dn] = (data *)malloc(sizeof(data));
										dblock[dn]->link = NULL;
										dblock[dn]->next = NULL;
										dblock[dn]->down = NULL;
										dblock[dn]->parent = -1;
										dblock[dn]->d_num = dn;
										dblock[dn]->type = 'i';

										dblock[ip[in].dib]->con.ind[dcount] = dn;

										scount = 0;
										dcount++;
										s_dn = dn;
									}

									dn = space_data(sp);
									bit_data(sp, dn);

									dblock[dn] = (data *)malloc(sizeof(data));
									dblock[dn]->link = NULL;
									dblock[dn]->next = NULL;
									dblock[dn]->down = NULL;
									dblock[dn]->parent = -1;
									dblock[dn]->d_num = dn;
									dblock[dn]->type = 'f';
									// 128바이트 내용 적기.
									i=0;
									while(fscanf(ofp, "%c", &(dblock[dn]->con.fc[i]))!=EOF&&i<128)
									{
										i++;
									}

									fclose(ofp);

									tmp = dblock[ip[in].db];
									while(tmp->link!=NULL)
									{
										tmp = tmp->link;
									}
									tmp->link = dblock[dn];

									dblock[s_dn]->con.ind[scount] = dn;

									scount++;
									count++;

								}
								name_write(in, now_d, sp, f2);
							}
						}
					}
				}
			}
		}
	}
}
void func_mymkdir(super *sp, inode *ip, char words[], struct tm *t, int now_d_num, inode *now_d)
{

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	int in, dn, i, s_dn, n_dn;
	int j, k, a;
	data *tmp;
	
	in = space_inode(sp);
	bit_inode(sp, in);

	dn = space_data(sp);
	bit_data(sp, dn);

	ip[in].file_type = "directory";
	ip[in].f_t.year = 1900+t->tm_year;
	ip[in].f_t.month = 1+t->tm_mon;
	ip[in].f_t.day = t->tm_mday;
	ip[in].f_t.hour = t->tm_hour;
	ip[in].f_t.minute = t->tm_min;
	ip[in].f_t.seconds = t->tm_sec;
	ip[in].db = dn;
	ip[in].sib = -1;
	ip[in].dib = -1;
	dblock[dn] = (data *)malloc(sizeof(data));
	dblock[dn]->con.dc[0][0] = '\0';
	dblock[dn]->next = NULL;
	dblock[dn]->down = NULL;
	dblock[dn]->link = NULL;
	dblock[dn]->parent = now_d_num;
	dblock[dn]->type = 'd';
	dblock[dn]->d_num = dn;

	// 부모, 형제 디렉터리와 연결
	if(dblock[now_d->db]->con.dc[0][0]=='\0')
		dblock[now_d->db]->down = dblock[dn];
	else
	{
		tmp = dblock[now_d->db]->down;
		while(tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = dblock[dn];
	}

	name_write(in, now_d, sp, words);
}
void func_myrmdir(super *sp, char words[], inode *ip, inode *now_d)
{
	int find, in, dn, stop = 0;
	int i, j, k;
	data *tmp;
	data *tmp2;

	find = name_find(ip, words, now_d);

	if(find == 0)   // 현재 디렉토리에 없음.
	{
		printf("myrmdir : failed to remove '%s' : No such file or directory\n", words);
	}
	else
	{
		in = find;  // in : 아이노드 값		

		if(strcmp(ip[in].file_type, "regular file")==0)
		{
			printf("myrmdir : failed to remove '%s' : Not a Directory\n", words);
		}
		else // 디렉토리임.
		{
			dn = ip[in].db;  // dn : 지우려는 디렉토리 첫 데이터블록 번호.
			if(dblock[dn]->con.dc[0][0] != '\0')
			{
				printf("myrmdir : failed to remove '%s' : Directory not empty\n", words);
			}
			else
			{
				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0] == in / 100 + 48)
							if(tmp->con.dc[i][1] == (in % 100) / 10 + 48)
								if(tmp->con.dc[i][2] == in % 10 + 48)
								{
									stop = 1;
									break;
								}
					}
					if(stop == 1)
						break;
					tmp = tmp -> link; // tmp 찾은 데이터블록
				}

				tmp2 = dblock[now_d->db];
				while(tmp2->link!=NULL)
				{
					tmp2 = tmp2 -> link;
				}
				
				for(j=0; tmp2->con.dc[j][0]!='\0'; ++j)
					;

				for(k=0; k<7; ++k)
					tmp->con.dc[i][k] = tmp2->con.dc[j-1][k];

				tmp2->con.dc[j-1][0] = '\0';

				// 맨 마지막 데이터블록이 비었는지 확인.
				if(now_d->sib!=-1)
				{
					if(tmp2->con.dc[0][0]=='\0')
					{
						tmp = dblock[0];
						while(tmp->link!=tmp2)
						{
							tmp = tmp -> link;
						}
						tmp -> link = NULL;
						dn = tmp2->d_num;

						free(dblock[dn]);  // 메모리 반납.
						bit_data(sp, dn);
						dblock[dn] = NULL;
						for(i=0; i<102; ++i)
							if(dblock[now_d->sib]->con.ind[i]==dn)
							{
								dblock[now_d->sib]->con.ind[i] = 0;
								break;
							}


						if(dblock[now_d->sib]->con.ind[0]==0) // 싱글 다 비어있음.
						{
							free(dblock[now_d->sib]);
							bit_data(sp, now_d->sib);
							dblock[now_d->sib] = NULL;
							now_d->sib = -1;
						}
						else
							;
					}
					else // 비어있지 않음.
						;

				}
				else  // single indirect가 없음.
					;

				// 아이노드에서 정보 삭제.
				dn = ip[in].db;

				ip[in].file_type = "";
				ip[in].f_t.year = 0;
				ip[in].f_t.month = 0;
				ip[in].f_t.day = 0;
				ip[in].f_t.hour = 0;
				ip[in].f_t.minute = 0;
				ip[in].f_t.seconds = 0;
				ip[in].file_size = 0;
				ip[in].db = 0;
				ip[in].sib = 0;
				ip[in].dib = 0;

				if((dblock[now_d->db]->down) == dblock[dn])
				{
					dblock[now_d->db]->down = dblock[dn]->next;
				}
				else
				{
					tmp = dblock[now_d->db]->down;
					while(tmp!=NULL)
					{
						if((tmp->next)==dblock[dn])
							break;
						tmp = tmp->next;
					}
					tmp->next = dblock[dn]->next;
				}

				free(dblock[dn]);

				bit_inode(sp, in);
				bit_data(sp, dn);

			}
		}
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
void func_myshowinode(char words[], inode *ip)
{
	int i, j, count=0, in=0;
	int num;
	data *tmp;

	if(words[0]==' '||words[0]=='\0')
		printf("Try input again\n");
	else
	{
		for(i=0; words[i]!='\0'; ++i)
			if(words[i]>='0'&& words[i]<='9')
				++count;
			else
			{
				count = -1;
				break;
			}

		if(count == -1)
			printf("Try input again\n");
		else
		{
			for(i=0; words[i]!='\0'; ++i)
			{
				num = words[i]-48;
				for(j=0; j<count-1; ++j)
					num *= 10;
				in += num;
				count--;
			}

			if(in>=1 && in<=512)
			{
				if(ip[in-1].file_type==NULL)
					printf("'%d' inode empty\n", in);
				else
				{
					printf("file type : %s\n", ip[in-1].file_type);
					printf("file size : %d\n", ip[in-1].file_size);
					printf("modified time : %d/%d/%d %d:%d:%d\n", ip[in-1].f_t.year, ip[in-1].f_t.month, ip[in-1].f_t.day, ip[in-1].f_t.hour, ip[in-1].f_t.minute, ip[in-1].f_t.seconds);
					printf("data block list :");
					
					tmp = dblock[ip[in-1].db];
					while(tmp!=NULL)
					{
						if(tmp->link==NULL)
							printf(" %d\n", (tmp->d_num)+1);
						else
							printf(" %d,", (tmp->d_num)+1);
						tmp = tmp->link;
					}
				}
			}
			else
				printf("Try input again\n");

		}
	}
}
void func_myshowblock(char words[], inode *ip)
{
	int i, j, count=0, dn=0, in = 0;
	int num, stop=0, p, d;
	data *tmp, *tmp2;

	if(words[0]==' '||words[0]=='\0')
		printf("Try input again\n");
	else
	{
		for(i=0; words[i]!='\0'; ++i)
			if(words[i]>='0'&& words[i]<='9')
				++count;
			else
			{
				count = -1;
				break;
			}

		if(count == -1)
			printf("Try input again\n");
		else
		{
			for(i=0; words[i]!='\0'; ++i)
			{
				num = words[i]-48;
				for(j=0; 
						j<count-1; ++j)
					num *= 10;
				dn += num;
				count--;
			}
			
			dn = dn - 1;
			if(dblock[dn]!=NULL)
			{
				if(dblock[dn]->type == 'd')
				{
					for(i=0; i<24; ++i)
					{
						if(dblock[dn]->con.dc[i][0]!='\0')
						{
							in=100*(dblock[dn]->con.dc[i][0]-48)+10*(dblock[dn]->con.dc[i][1]-48)+dblock[dn]->con.dc[i][2]-48;
							printf("%d	", in);
							for(j=3; j<7&&dblock[dn]->con.dc[i][j]!='\0'; ++j)
							{
								printf("%c", dblock[dn]->con.dc[i][j]);
							}
							printf("\n");
						}
						else
							break;
					}
				}
				else if(dblock[dn]->type == 'i')
				{
					for(i=0; dblock[dn]->con.ind[i]!=0; ++i)
						printf("%d", dblock[dn]->con.ind[i]+1);
					printf("\n");
				}
				else
				{
					for(i=0; i<128&&dblock[dn]->con.fc[i]!='\0'; ++i)
						printf("%c", dblock[dn]->con.fc[i]);
					printf("\n");
				}
			}
			else
				printf("'%d' data block empty\n", dn+1);
		}
	}
}



			

int space_inode(super *sp)
{
	int i, j, quotient, stop=0;
	int bit[8];

	for(i=0; i<64; ++i)
	{
		quotient = sp->inode_use[i];
		for(j=0; j<8; ++j)
		{
			bit[j] = quotient % 2;
			quotient = quotient / 2;
		}
		for(j=0; j<8; ++j)
			if(bit[j]==0)
			{
				stop = 1;
				break;
			}
		if(stop == 1)
			break;
	}

	j = 8 * i + j;

	return j;
}
int space_data(super *sp)
{
	int i, j, quotient, stop = 0;
	int bit[8];

	for(i=0; i<512; ++i)
	{
		quotient = sp->datablock_use[i];
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

	j = 8 * i + j;
	return j;
}
void bit_inode(super *sp, int in)
{
	int i, j;

	i = in / 8;
	j = in % 8;

	sp->inode_use[i] = (sp->inode_use[i]) ^ (1 << j);
}
void bit_data(super *sp, int dn)
{
	int i, j;

	i = dn / 8;
	j = dn % 8;

	sp->datablock_use[i] = (sp->datablock_use[i]) ^ (1 << j);
}
int name_find(inode *ip, char words[], inode *now_d)
{
	int i, j, k, l;
	int find = 0, num;
	int stop = 0;
	char dir_name[5];
	data *tmp;

	k = now_d->db;
	tmp = dblock[k];
	while(tmp!=NULL)
	{
		for(j=0; j<24; ++j)
		{
			if(tmp->con.dc[j][0]=='\0')
			{
				stop = 1;
				break;
			}
			else
			{
				for(l=0; (l<4)&&(tmp->con.dc[j][l+3]!='\0'); ++l)
					dir_name[l] = tmp->con.dc[j][l+3];
				dir_name[l] = '\0';
				
				if(strcmp(dir_name, words)==0)
				{
					find = 1;
					num = 100 * (tmp->con.dc[j][0] - 48) + 10 * (tmp->con.dc[j][1] - 48) + (tmp->con.dc[j][2] - 48);
					break;
				}

			}
		}
		if(find == 1)
			break;

		tmp = tmp->link;
	}

	if(find == 1)
		return num;
	else
		return 0;
}
void name_write(int in, inode *now_d, super *sp, char words[])
{
	int i, j, k, a, s_dn, n_dn;
	data *tmp;


	for(i=0; i<24; ++i)
		if(dblock[now_d->db]->con.dc[i][0]=='\0')
			break;
	if(i!=24)
	{
		dblock[now_d->db]->con.dc[i][0] = in / 100 + 48;
		dblock[now_d->db]->con.dc[i][1] = (in % 100) / 10 + 48;
		dblock[now_d->db]->con.dc[i][2] = (in % 10) + 48;

		for(j=0; words[j]!='\0'; ++j)
			dblock[now_d->db]->con.dc[i][j+3] = words[j];
		dblock[now_d->db]->con.dc[i][j+3] = '\0';
		dblock[now_d->db]->con.dc[i+1][0] = '\0';

	}
	else
	{
		if(now_d->sib == -1)
		{
			s_dn = space_data(sp);
			bit_data(sp, s_dn);

			dblock[s_dn] = (data *)malloc(sizeof(data));
			now_d->sib = s_dn;
			dblock[s_dn]->con.ind[0] = 0;
			dblock[s_dn]->next = NULL;
			dblock[s_dn]->down = NULL;
			dblock[s_dn]->link = NULL;
			dblock[s_dn]->parent = -1;
			dblock[s_dn]->d_num = s_dn;
			dblock[s_dn]->type = 'i';

			n_dn = space_data(sp);
			bit_data(sp, n_dn);

			dblock[n_dn] = (data *)malloc(sizeof(data));
			dblock[n_dn]->next = NULL;
			dblock[n_dn]->down = NULL;
			dblock[n_dn]->link = NULL;
			dblock[n_dn]->parent = -1;
			dblock[n_dn]->type = 'd';
			dblock[n_dn]->d_num = n_dn;

			dblock[n_dn]->con.dc[0][0] = in / 100 + 48;
			dblock[n_dn]->con.dc[0][1] = (in % 100) / 10 + 48;
			dblock[n_dn]->con.dc[0][2] = in % 10 + 48;

			for(i=0; words[i] !='\0'; ++i)
				dblock[n_dn]->con.dc[0][i+3] = words[i];
			dblock[n_dn]->con.dc[0][i+3] = '\0';
			dblock[n_dn]->con.dc[1][0] = '\0';

			dblock[s_dn]->con.ind[0] = n_dn;
			dblock[s_dn]->con.ind[1] = 0;

			tmp = dblock[now_d->db];
			while(tmp->link != NULL)
			{
				tmp = tmp->link;
			}
			tmp->link = dblock[n_dn];

		}
		else
		{
			for(i=0; i<102; ++i)
				if(dblock[now_d->sib]->con.ind[i]==0)
					break;

			// i 값 : single indirect 에서 0인 부분
			a = dblock[now_d->sib]->con.ind[i-1];
			for(j=0; j<24; ++j)
				if(dblock[a]->con.dc[j][0] == '\0')
					break;


			if(j!=24)
			{
				dblock[a]->con.dc[j][0] = in / 100 + 48;
				dblock[a]->con.dc[j][1] = (in % 100) / 10 + 48;
				dblock[a]->con.dc[j][2] = in % 10 + 48;

				for(k=0; words[k]!='\0'; ++k)
					dblock[a]->con.dc[j][k+3] = words[k];
				dblock[a]->con.dc[j][k+3] = '\0';
				dblock[a]->con.dc[j+1][0] = '\0';
			}
			else
			{
				n_dn = space_data(sp);
				bit_data(sp, n_dn);

				dblock[n_dn] = (data *)malloc(sizeof(data));
				dblock[n_dn]->next = NULL;
				dblock[n_dn]->down = NULL;
				dblock[n_dn]->link = NULL;
				dblock[n_dn]->parent = -1;
				dblock[n_dn]->type = 'd';
				dblock[n_dn]->d_num = n_dn;

				dblock[n_dn]->con.dc[0][0] = in / 100 + 48;
				dblock[n_dn]->con.dc[0][1] = (in % 100) / 10 + 48;
				dblock[n_dn]->con.dc[0][2] = in % 10 + 48;

				for(j=0; words[j]!='\0'; ++j)
					dblock[n_dn]->con.dc[0][j+3] = words[j];
				dblock[n_dn]->con.dc[0][j+3] = '\0';
				dblock[n_dn]->con.dc[1][0] = '\0';

				dblock[now_d->sib]->con.ind[i] = n_dn;
				dblock[now_d->sib]->con.ind[i+1] = 0;

				tmp = dblock[now_d->db];
				while(tmp->link != NULL)
				{
					tmp = tmp->link;
				}
				tmp->link = dblock[n_dn];
			}
		}
	}
}
