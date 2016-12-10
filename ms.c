#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
union file_contents {
	char dc[24][7];
	char fc[129];
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
	int seconds;
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
char mypwd[100];
int update_mymkfs(super *, inode *, inode **);
int func_mymkfs(super *, inode *, struct tm *, inode **); // 0) mymkfs
void func_myls(inode *, char [], inode *now_d, int); // 1) myls
void func_mycat(char [], inode *, inode *, super *, struct tm *, int); // 2) mycat
void func_myshowfile(inode *, char [], inode *); // 3) myshowfile
int func_mycd(inode *, char [], inode *, inode **, int); // 5) mycd
void func_mycp(char [], char [], inode *, int, super *, inode *, struct tm *); // 6) mycp
void func_mycpto(inode *, char [], inode *); // 7) mycpto
void func_mycpfrom(super *, inode *, char [], struct tm *, int, inode *); // 8)mycpfrom
void func_mymkdir(super *, inode *, char [], struct tm *, int, inode *); // 9) mymkdir
void func_myrmdir(super *, char [], inode *, inode *, int);  // 10) myrmdir
void func_myrm(super *, char [], inode *, inode *, int); // 11) myrm
void func_mymv(char [], inode *, inode *, int, super *); // 12) mymv
void func_mytouch(char [], super *, inode *, inode *, struct tm *, int); // 13) mytouch
void func_myshowinode(char [], inode *);  // 14) myshowinode
void func_myshowblock(char [], inode *); // 15) myshowblock
void func_mystate(super *); // 16 ) mystate
void func_mytree(char[], inode *, inode *, int); // 17) mytree
void print_tree(inode *, int, data *);
int space_inode(super *);
int space_data(super *);
void bit_inode(super *, int);
void bit_data(super *, int);
int name_find(inode *, char [], inode *);
void name_write(int, inode *, super *, char []);
int find_root(inode *, char[], inode *, int, int *, inode **, int *);
void name_remove(inode *, super *, int, int);
void remove_nowd(char []);
void final_func(super *);
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
			if(strcmp(mypwd, "/")==0)
				printf("[%s ]$ ", mypwd);
			else
				printf("[%s/ ]$ ", mypwd);
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
					func_myls(inode_list, words, now_d, now_d_num);
				}
				else if(co_num == 2)
				{
					func_mycat(words, inode_list, now_d, &superblock, t, now_d_num);
				}
				else if(co_num == 3)
				{
					func_myshowfile(inode_list, words, now_d);
				}
				else if(co_num == 4)
				{
					if(mypwd[0] == '\0')
					{
						mypwd[0] = '/';
						mypwd[1] = '\0';
					}

					printf("%s\n", mypwd);

				}
				else if(co_num == 5)
				{
					int check;

					check = func_mycd(inode_list, words, now_d, &now_d, now_d_num);
					now_d_num = check;
				}
				else if(co_num == 6)
				{
					char f1[5], f2[5];

					for(i=0; i<4&&words[i]!=' '; ++i)
						f1[i] = words[i];
					f1[i] = '\0';

					for(i=0; words[i]!=' '; ++i)
						;

					for(j=0, i=i+1; j<4&&words[i]!='\0'; ++i)
					{
						f2[j] = words[i];
						j++;
					}
					f2[j] = '\0';

					func_mycp(f1, f2, inode_list, now_d_num, &superblock, now_d, t);
				}
				else if(co_num == 7)
				{
					func_mycpto(inode_list, words, now_d);
				}
				else if(co_num == 8)
				{
					func_mycpfrom(&superblock, inode_list, words, t, now_d_num, now_d);
				}
				else if(co_num == 9)
				{
					func_mymkdir(&superblock, inode_list, words, t, now_d_num, now_d);
				}
				else if(co_num == 10)
				{
					func_myrmdir(&superblock, words, inode_list, now_d, now_d_num);
				}
				else if(co_num == 11)
				{
					func_myrm(&superblock, words, inode_list, now_d, now_d_num);
				}
				else if(co_num == 12)
				{
					func_mymv(words, inode_list, now_d, now_d_num, &superblock);
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
					func_mytree(words, inode_list, now_d, now_d_num);
				}
				else // co_num == 18(cygwin 명령어 실행)
				{
					if(strcmp(compare, "byebye")==0)
					{
						final_func(&superblock);
						exit(0);
					}
					else
					{
						for(i=0; compare[i]!='\0'; ++i)
							;
						if(words[0] == '\n')
							system(compare);
						else
						{
							compare[i] = ' ';
							for(j=i; words[j-i]!='\0'; ++j)
								compare[j+1] = words[j-i];
							compare[j+1] = '\0';
							system(compare);
						}

					}
				}
			}
			else
				printf("error : no myfs\n");

			printf("\n");
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
	mypwd[0] = '/';
	mypwd[0] = '\0';

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

	mypwd[0] = '/';
	mypwd[1] = '\0';

	return 0; // 부모 디렉터리의 아이노드 값.
}
void func_mymv(char words[], inode *ip, inode *now_d, int now_d_num, super *sp)
{
	char r1[100], r2[100], w1[100], w2[100];
	data *tmp, *tmp1;
	inode *pd1, *pd2;
	int pnd1, pnd2, nd1, nd2, i, j, k;
	int space = 0, count = 0;
	int find1, find2, in1, in2;
	int stop = 0;

	for(i=0; words[i]!='\0'; ++i)
		if(words[i] == ' ')
			space++;

	if(words[0] == '\0' || words[0] == ' ')
		printf("Try input again\n");
	else if(space != 1)
		printf("mymv : failed to move : missing operand\n");
	else if(words[0] == '/' && words[1] == ' ')
		printf("Try input again\n");
	else
	{
		for(i=0; words[i] != ' '; ++i)
			w1[i] = words[i];
		w1[i] = '\0';

		for(i=i+1, j=0; words[i]!='\0'; ++i)
		{
			w2[j] = words[i];
			j++;
		}
		w2[j] = '\0';

		if(w1[0] != '/')
		{
			strcpy(r1, mypwd);

			for(i=0; r1[i]!='\0'; ++i)
				;

			if(r1[i-1] != '/')
			{
				r1[i] = '/';
				r1[i+1] = '\0';
			}

			for(i=0; r1[i]!='\0'; ++i)
				;

			for(j=0; w1[j]!='\0'; ++j)
			{
				r1[i] = w1[j];
				i++;
			}
			r1[i] = '\0';

			strcpy(w1, r1);
		}
		else
			strcpy(r1, w1);

		if(w2[0] != '/')
		{
			strcpy(r2, mypwd);

			for(i=0; r2[i]!='\0'; ++i)
				;

			if(r2[i-1]!='/')
			{
				r2[i] = '/';
				r2[i+1] = '\0';
			}

			for(i=0; r2[i]!='\0'; ++i)
				;

			for(j=0; w2[j]!='\0'; ++j)
			{
				r2[i] = w2[j];
				i++;
			}
			r2[i] = '\0';

			strcpy(w2, r2);
		}
		else
			strcpy(r2, w2);

		remove_nowd(r1);
		remove_nowd(r2);

		if(r1[0] == '\0')
		{
			r1[0] = '/';
			r1[1] = '\0';
			strcpy(w1, r1);
		}
		if(r2[0] == '\0')
		{
			r2[0] = '/';
			r2[1] = '\0';
			strcpy(w2, r2);
		}

		if(strcmp(r1, "/") == 0)
			printf("Try input again\n");
		else
		{
			find1 = find_root(ip, w1, now_d, now_d_num, &pnd1, &pd1, &nd1);

			while(1)
			{
				stop = 0;
				for(i=0; r1[i]!='\0'; ++i)
					if(r1[i] == '/')
						if(r1[i+1] == '.')
							if(r2[i+2] == '.')
							{
								stop = 1;
								break;
							}
				if(stop == 1)
				{
					for(j=i-1; j>0; --j)
						if(r1[j]=='/')
							break;

					for(k=i+1; r1[k]!='\0'; ++k)
						if(r1[k]=='/')
							break;

					for(; r1[k]!='\0'; ++k)
					{
						r1[j] = r1[k];
						j++;
					}
					r1[j] = '\0';
				}
				else
					break;
			}

			find1 = find_root(ip, r1, now_d, now_d_num, &pnd1, &pd1, &nd1);

			for(i=0; i<4&&r1[i]!='\0'; ++i)
				r1[i] = r1[i];
			r1[i] = '\0';

			if(find1 == 0)
				printf("mymv : failed to move : '%s' is not exists\n", r1);
			else if(find1 == 3)
				printf("mymv : failed to move : 잘못된 경로입니다.\n");
			else
			{
				while(1)
				{
					stop = 0;
					for(i=0; w2[i]!='\0'; ++i)
						if(w2[i] == '/')
							if(w2[i+1] == '.')
								if(w2[i+2] = '.')
								{
									stop = 1;
									break;
								}

					if(stop == 1)
					{
						for(j=i-1; j>0; --j)
							if(w2[j]=='/')
								break;

						for(k=i+1; w2[k]!='\0'; ++k)
							if(w2[k] == '/')
								break;

						for(; w2[k]!='\0'; ++k)
						{
							w2[j] = w2[k];
							j++;
						}
						w2[j] = '\0';
					}
					else
						break;
				}

				strcpy(r2, w2);

				if(strcmp(r2, "/")==0)
				{
					find2 = 1;
					pnd2 = 0;
					pd2 = &ip[0];
				}
				else
				{
					find2 = find_root(ip, w2, now_d, now_d_num, &pnd2, &pd2, &nd2);
				}

				for(i=0; i<4&&w2[i]!='\0'; ++i)
					r2[i] = w2[i];
				r2[i] = '\0';

				in1 = name_find(ip, r1, pd1);
				in2 = name_find(ip, r2, pd2);

				if(find2 == 0)
				{
					name_remove(pd1, sp, 0, in1);
					name_write(in1, pd2, sp, r2);

					dblock[ip[in1].db]->parent = pnd2;

					tmp1 = dblock[pd1->db];
					tmp = dblock[ip[in1].db];
					if(tmp1->down == tmp)
					{
						tmp1->down = tmp->next;
					}
					else
					{
						tmp1 = tmp1->down;
						while(tmp1->next!=tmp)
						{
							tmp1 = tmp1->next;
						}
						tmp1->next = tmp->next;
					}

					tmp1 = dblock[pd2->db];
					if(tmp1->down == NULL)
					{
						tmp1->down = tmp;
						tmp->next = NULL;
					}
					else
					{
						tmp1 = tmp1->down;
						while(tmp1->next!=NULL)
						{
							tmp1 = tmp1->next;
						}
						tmp1->next = tmp;
						tmp->next = NULL;
					}
				}
				else if(find2 == 3)
				{
					printf("mymv : failed to move : 잘못된 경로입니다.\n");
				}
				else if(find2 == 1)
				{
					name_remove(pd1, sp, 0, in1);
					name_write(in1, &ip[in2], sp, r1);
					
					dblock[ip[in1].db]->parent = in2;

					tmp1 = dblock[pd1->db];
					tmp = dblock[ip[in1].db];
					if(tmp1->down == tmp)
						tmp1->down = tmp->next;
					else
					{
						tmp1 = tmp1->down;
						while(tmp1->next!=tmp)
						{
							tmp1 = tmp1->next;
						}
						tmp1->next = tmp->next;
					}

					tmp1 = dblock[ip[in2].db];
					if(tmp1->down == NULL)
					{
						tmp1->down = tmp;
						tmp->next = NULL;
					}
					else
					{
						tmp1 = tmp1->down;
						while(tmp1->next != NULL)
						{
							tmp1 = tmp1->next;
						}
						tmp1->next = tmp;
						tmp->next = NULL;
					}
				}
				else
				{
					printf("mymv : failed to move : '%s' is already exists\n", r2);
				}
			}
		}
	}
}
void func_mycat(char words[], inode *ip, inode *now_d, super *sp, struct tm *t, int now_d_num)
{
	char f1[5], f2[5];
	int i, j, stop = 0;
	int in;
	data *tmp;

	for(i=0; words[i]!='\0'; ++i)
	{
		if(words[i] == '>')
		{
			stop = 1;
			break;
		}
		else if(words[i] == '<')
		{
			stop = 2;
			break;
		}
		else
			;
	}

	if(stop == 0)
	{
		if(words[0] == ' '||words[0] == '\0')
			printf("Try input again\n");
		else
		{
			for(i=0; i<4&&words[i]!='\0'; ++i)
				words[i] = words[i];
			words[i] = '\0';

			in = name_find(ip, words, now_d);

			if(in == 0)
				printf("mycat : failed to show : File not exists\n");
			else
			{
				if(strcmp(ip[in].file_type, "directory")==0)
					printf("mycat : failed to show : '%s' is directory\n", words);
				else
				{
					tmp = dblock[ip[in].db];

					while(tmp!=NULL)
					{
						printf("%s", tmp->con.fc);
						tmp = tmp->link;
					}
				}
			}
		}
	}
	else if(stop == 2)
		printf("mycat : don't use '<' (please use '>')\n");
	else
	{
		int count = 0;

		for(i=0; words[i]!='>'; ++i)
			if(words[i] == ' ')
				count++;

		if(count == 1)
		{
			for(i=0; i<4&&words[i]!=' '; ++i)
				f1[i] = words[i];
			f1[i] = '\0';

			for(i=0; words[i]!='>'; ++i)
				;

			for(i=i+2, j=0; j<4&&words[i]!='\0'; ++i)
			{
				f2[j] = words[i];
				j++;
			}
			f2[j] = '\0';

			func_mycp(f1, f2, ip, now_d_num, sp, now_d, t);
		}
		else if(count == 2)
		{
			char f3[5];

			for(i=0; i<4&&words[i]!=' '; ++i)
				f1[i] = words[i];
			f1[i] = '\0';

			for(i=0; words[i]!=' '; ++i)
				;

			for(i=i+1, j=0; j<4&&words[i]!=' '; ++i)
			{
				f2[j] = words[i];
				j++;
			}
			f2[j] = '\0';

			for(i=0; words[i]!='>'; ++i)
				;

			for(i=i+2, j=0; j<4&&words[i]!='\0'; ++i)
			{
				f3[j] = words[i];
				j++;
			}
			f3[j] = '\0';

			int in1, in2, check1, check2;

			in1 = name_find(ip, f1, now_d);

			in2 = name_find(ip, f2, now_d);

			if(in1 == 0 && in2 == 0)
				printf("mycat : failed to mycat : '%s', '%s' is not exists\n", f1, f2);
			else if(in1 == 0 && in2 != 0)
				printf("mycat : failed to mycat : '%s' is not exists\n", f1);
			else if(in1 != 0 && in2 == 0)
				printf("mycat : failed to mycat : '%s' is not exists\n", f2);
			else
			{
				check1 = strcmp(ip[in1].file_type, "regular file");
				check2 = strcmp(ip[in2].file_type, "regular file");

				if(check1 == 0 && check2 == 0)
				{
					char w[100];
					FILE *ofp;

					strcpy(w, f1);
					for(i=0; w[i]!='\0'; ++i)
						;
					w[i] = ' ';

					w[i+1] = 'y';
					w[i+2] = 'i';
					w[i+3] = '\0';

					func_mycpto(ip, w, now_d);

					ofp = fopen("yi", "a");

					tmp = dblock[ip[in2].db];
					while(tmp!=NULL)
					{
						for(i=0; tmp->con.fc[i]!='\0'; ++i)
							fprintf(ofp, "%c", tmp->con.fc[i]);
						tmp = tmp->link;
					}

					fclose(ofp);

					w[0] = 'y';
					w[1] = 'i';
					w[2] = ' ';

					for(i=3; f3[i-3]!='\0'; ++i)
						w[i] = f3[i-3];
					w[i] = '\0';

					func_mycpfrom(sp, ip, w, t, now_d_num, now_d);
					system("rm yi");

				}
				else if(check1 == 0 && check2 !=0)
				{
					printf("mycat : failed to mycat : '%s' is directory\n", f2);
				}
				else if(check1 != 0 && check2 == 0)
				{
					printf("mycat : failed to mycat : '%s' is directory\n", f1);
				}
				else
					printf("mycat : failed to mycat : '%s', '%s' is directory\n", f1, f2);
			}
		}
		else
			printf("Try input again\n");
	}
}
void func_mycp(char f1[], char f2[], inode *ip, int now_d_num, super *sp, inode *now_d, struct tm *t)
{
	int i, j, in;
	int nin, ndn, byte, num;
	int dcount=0, scount=0, count=0, s_dn=0, check=0;
	data *tmp;
	data *tmp2;

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	in = name_find(ip, f1, now_d);

	if(in == 0)
		printf("mycp : failed to copy : '%s' not exists\n", f1);
	else
	{
		if(strcmp(ip[in].file_type, "directory")==0)
			printf("mycp : failed to copy : '%s' is directory\n", f1);
		else
		{
			check = name_find(ip, f2, now_d);

			if(check != 0)
				printf("mycp : failed to copy : '%s' is already exists\n", f2);
			else
			{
				byte = ip[in].file_size;

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
					nin = space_inode(sp);
					bit_inode(sp, nin);

					ip[nin].file_type = "regular file";
					ip[nin].f_t.year = 1900+t->tm_year;
					ip[nin].f_t.month = 1+t->tm_mon;
					ip[nin].f_t.day = t->tm_mday;
					ip[nin].f_t.hour = t->tm_hour;
					ip[nin].f_t.minute = t->tm_min;
					ip[nin].f_t.seconds = t->tm_sec;
					ip[nin].file_size = byte;

					ndn = space_data(sp);
					bit_data(sp, ndn);

					ip[nin].db = ndn;
					ip[nin].sib = -1;
					ip[nin].dib = -1;
					dblock[ndn] = (data *)malloc(sizeof(data));
					dblock[ndn]->link = NULL;
					dblock[ndn]->next = NULL;
					dblock[ndn]->down = NULL;
					dblock[ndn]->parent = now_d_num;
					dblock[ndn]->d_num = ndn;
					dblock[ndn]->type = 'f';
					// 데이터블록 1개에 내용 넣기.
					for(i=0; dblock[ip[in].db]->con.fc[i]!='\0'; ++i)
						dblock[ndn]->con.fc[i] = dblock[ip[in].db]->con.fc[i];
					dblock[ndn]->con.fc[i] = '\0';

					// 형제 or 부모 찾아서 연결
					if(dblock[now_d->db]->con.dc[0][0]=='\0')
						dblock[now_d->db]->down = dblock[ndn];
					else
					{
						tmp = dblock[now_d->db]->down;
						while(tmp->next!=NULL)
						{
							tmp = tmp->next;
						}
						tmp->next = dblock[ndn];
					}
					name_write(nin, now_d, sp, f2);
				}
				else if((num>1)&&(num<104))
				{
					nin = space_inode(sp);
					bit_inode(sp, nin);

					ip[nin].file_type = "regular file";
					ip[nin].f_t.year = 1900+t->tm_year;
					ip[nin].f_t.month = 1+t->tm_mon;
					ip[nin].f_t.day = t->tm_mday;
					ip[nin].f_t.hour = t->tm_hour;
					ip[nin].f_t.minute = t->tm_min;
					ip[nin].f_t.seconds = t->tm_sec;
					ip[nin].file_size = byte;

					// 첫번째 
					// 데이터블록 생성.
					ndn = space_data(sp);
					bit_data(sp, ndn);

					ip[nin].db = ndn;
					ip[nin].sib = -1;
					ip[nin].dib = -1;

					dblock[ndn] = (data *)malloc(sizeof(data));
					dblock[ndn]->next = NULL;
					dblock[ndn]->link = NULL;
					dblock[ndn]->down = NULL;
					dblock[ndn]->parent = now_d_num;
					dblock[ndn]->d_num = ndn;
					dblock[ndn]->type = 'f';
					//128바이트까지 첫번째 데이터블록에 내용 넣기.
					tmp2 = dblock[ip[in].db];
					for(i=0; tmp2->con.fc[i]!='\0'; ++i)
						dblock[ndn]->con.fc[i] = tmp2->con.fc[i];
					dblock[ndn]->con.fc[i] = '\0';

					//부모 or 형제 디렉터리와 연결
					if(dblock[now_d->db]->con.dc[0][0]=='\0')
						dblock[now_d->db]->down = dblock[ndn];
					else
					{
						tmp = dblock[now_d->db]->down;
						while(tmp->next!=NULL)
						{
							tmp = tmp->next;
						}
						tmp -> next = dblock[ndn];
					}

					// single indirect 데이터블록 만들기
					ndn = space_data(sp);
					bit_data(sp, ndn);

					dblock[ndn] = (data *)malloc(sizeof(data));
					dblock[ndn]->con.ind[0] = 0;
					dblock[ndn]->next = NULL;
					dblock[ndn]->link = NULL;
					dblock[ndn]->down = NULL;
					dblock[ndn]->parent = -1;
					dblock[ndn]->d_num = ndn;
					dblock[ndn]->type = 'i';
					ip[nin].sib = ndn;

					count = 2;
					while(count<num+1)
					{
						tmp2 = tmp2->link;
						ndn = space_data(sp);
						bit_data(sp, ndn);

						dblock[ndn] = (data *)malloc(sizeof(data));
						dblock[ndn]->next = NULL;
						dblock[ndn]->link = NULL;
						dblock[ndn]->down = NULL;
						dblock[ndn]->parent = -1;
						dblock[ndn]->d_num = ndn;
						dblock[ndn]->type = 'f';

						i=0;
						tmp = dblock[ip[nin].db];
						while(tmp->link!=NULL)
						{
							tmp = tmp->link;
						}
						tmp->link = dblock[ndn];

						for(i=0; tmp2->con.fc[i]!='\0'; ++i)
							dblock[ndn]->con.fc[i] = tmp2->con.fc[i];
						dblock[ndn]->con.fc[i] = '\0';

						dblock[ip[nin].sib]->con.ind[count-2] = ndn;
						if(count != 103)
							dblock[ip[nin].sib]->con.ind[count-1] = 0;

						count++;
					}
					name_write(nin, now_d, sp, f2);

				}
				else
				{
					nin = space_inode(sp);
					bit_inode(sp, nin);

					ip[nin].file_type = "regular file";
					ip[nin].f_t.year = 1900+t->tm_year;
					ip[nin].f_t.month = 1+t->tm_mon;
					ip[nin].f_t.day = t->tm_mday;
					ip[nin].f_t.hour = t->tm_hour;
					ip[nin].f_t.minute = t->tm_min;
					ip[nin].f_t.seconds = t->tm_sec;
					ip[nin].file_size = byte;

					// 첫번째 데이터 블록 생성.
					ndn = space_data(sp);
					bit_data(sp, ndn);

					ip[nin].db = ndn;
					ip[nin].sib = -1;
					ip[nin].dib = -1;

					dblock[ndn] = (data *)malloc(sizeof(data));
					dblock[ndn]->next = NULL;
					dblock[ndn]->link = NULL;
					dblock[ndn]->down = NULL;
					dblock[ndn]->parent = now_d_num;
					dblock[ndn]->d_num = ndn;
					dblock[ndn]->type = 'f';

					tmp2 = dblock[ip[in].db];
					for(i=0; tmp2->con.fc[i]!='\0'; ++i)
						dblock[ndn]->con.fc[i] = tmp2->con.fc[i];
					dblock[ndn]->con.fc[i] = '\0';


					//부모 or 형제 디렉터리와 연결
					if(dblock[now_d->db]->con.dc[0][0]=='\0')
						dblock[now_d->db]->down = dblock[ndn];
					else
					{
						tmp = dblock[now_d->db]->down;
						while(tmp->next!=NULL)
						{
							tmp = tmp->next;
						}
						tmp->next = dblock[ndn];
					}

					// single indirect 데이터블록 만들기
					ndn = space_data(sp);
					bit_data(sp, ndn);

					dblock[ndn] = (data *)malloc(sizeof(data));
					dblock[ndn]->con.ind[0] = 0;
					dblock[ndn]->next = NULL;
					dblock[ndn]->link = NULL;
					dblock[ndn]->down = NULL;
					dblock[ndn]->parent = -1;
					dblock[ndn]->d_num = ndn;
					dblock[ndn]->type = 'i';
					ip[nin].sib = ndn;

					count = 2;
					while(count<104)
					{
						tmp2 = tmp2 -> link;
						ndn = space_data(sp);
						bit_data(sp, ndn);

						dblock[ndn] = (data *)malloc(sizeof(data));
						dblock[ndn]->next = NULL;
						dblock[ndn]->link = NULL;
						dblock[ndn]->down = NULL;
						dblock[ndn]->parent = -1;
						dblock[ndn]->d_num = ndn;
						dblock[ndn]->type = 'f';
						//128바이트 내용 넣기
						for(i=0; tmp2->con.fc[i]!='\0'; ++i)
							dblock[ndn]->con.fc[i] = tmp2->con.fc[i];
						dblock[ndn]->con.fc[i] = '\0';

						tmp = dblock[ip[nin].db];
						while(tmp->link!=NULL)
						{
							tmp = tmp->link;
						}
						tmp->link = dblock[ndn];

						dblock[ip[nin].sib]->con.ind[count-2] = ndn;
						if(count != 103)
							dblock[ip[nin].sib]->con.ind[count-1] = 0;

						count++;
					}

					ndn = space_data(sp);
					bit_data(sp, ndn);

					dblock[ndn] = (data *)malloc(sizeof(data));
					dblock[ndn]->link = NULL;
					dblock[ndn]->next = NULL;
					dblock[ndn]->down = NULL;
					dblock[ndn]->type = 'i';
					ip[nin].dib = ndn;
					dblock[ndn]->con.ind[0] = 0;

					int j = 1;
					while(count < num+1)
					{
						tmp2 = tmp2->link;
						if(count % 102 == 2)
						{
							ndn = space_data(sp);
							bit_data(sp, ndn);

							dblock[ndn] = (data *)malloc(sizeof(data));
							dblock[ndn]->link = NULL;
							dblock[ndn]->next = NULL;
							dblock[ndn]->down = NULL;
							dblock[ndn]->parent = -1;
							dblock[ndn]->d_num = ndn;
							dblock[ndn]->type = 'i';

							dblock[ip[nin].dib]->con.ind[dcount] = ndn;
							if(dcount+1!=102)
								dblock[ip[nin].dib]->con.ind[dcount+1] = 0;

							scount = 0;
							dcount++;
							s_dn = ndn;
						}

						ndn = space_data(sp);
						bit_data(sp, ndn);

						dblock[ndn] = (data *)malloc(sizeof(data));
						dblock[ndn]->link = NULL;
						dblock[ndn]->next = NULL;
						dblock[ndn]->down = NULL;
						dblock[ndn]->parent = -1;
						dblock[ndn]->d_num = ndn;
						dblock[ndn]->type = 'f';
						// 128바이트 내용 적기.
						i=0;
						for(i=0; tmp2->con.fc[i]!='\0'; ++i)
							dblock[ndn]->con.fc[i] = tmp2->con.fc[i];
						dblock[ndn]->con.fc[i] = '\0';

						tmp = dblock[ip[nin].db];
						while(tmp->link!=NULL)
						{
							tmp = tmp->link;
						}
						tmp->link = dblock[ndn];

						dblock[s_dn]->con.ind[scount] = ndn;
						if(scount+1!=102)
							dblock[s_dn]->con.ind[scount+1] = 0;

						scount++;
						count++;

					}
					name_write(nin, now_d, sp, f2);
				}
			}
		}
	}
}
void func_myls(inode *ip, char words[], inode *now_d, int now_d_num)
{
	data *tmp;
	int i, count = 0;
	int stop = 0, j, find = 0, k, in;
	int pnd, nd, count1;
	int ino;
	char a[5], n[4];
	inode *pd;

	if(words[0]=='\0')
	{
		tmp = dblock[now_d->db];
		while(tmp!=NULL)
		{
			for(i=0; i<24; ++i)
			{
				if(tmp->con.dc[i][0]=='\0')
				{
					stop = 1;
					break;
				}
				++count;
			}
			tmp = tmp->link;
		}

		char name[count][5];

		stop = 0;
		int k = 0;
		tmp = dblock[now_d->db];
		while(tmp!=NULL)
		{
			for(i=0; i<24; ++i)
			{
				if(tmp->con.dc[i][0]=='\0')
				{
					stop = 1;
					break;
				}

				for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
					name[k][j-3] = tmp->con.dc[i][j];
				name[k++][j-3] = '\0';
			}
			tmp = tmp->link;
		}

		count1 = count;
		for(i=count-1; i>0; --i)
		{
			for(j=0; j<count-1; ++j)
				if(strcmp(name[j], name[j+1])>0)
				{
					strcpy(a, name[j]);
					strcpy(name[j], name[j+1]);
					strcpy(name[j+1], a);
				}
			count --;
		}

		printf(".\n");
		printf("..\n");
		for(i=0; i<count1; ++i)
			printf("%s\n", name[i]);
	}
	else if(words[0] == '-')
	{
		if((words[1]=='i'&&words[2]=='l')||(words[1] == 'l'&&words[2]=='i'))
		{
			if(words[3] == '\0')
			{
				stop = 0;
				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}
						count++;
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				char name[count][5];
				char num[count][4];

				stop = 0;
				k=0;
				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}

						for(j=0; j<3; ++j)
							num[k][j] = tmp->con.dc[i][j];
						num[k][j] = '\0';

						for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
							name[k][j-3] = tmp->con.dc[i][j];
						name[k++][j-3] = '\0';
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				count1 = count;
				for(i=count-1; i>0; --i)
				{
					for(j=0; j<count-1; ++j)
					{
						if(strcmp(name[j], name[j+1])>0)
						{
							strcpy(a, name[j]);
							strcpy(name[j], name[j+1]);
							strcpy(name[j+1], a);

							strcpy(n, num[j]);
							strcpy(num[j], num[j+1]);
							strcpy(num[j+1], n);
						}
					}
					count--;
				}

				if(now_d->db==0)
				{
					printf("%3d d %d\t%d/%d/%d %d:%d:%d .\n", 1, ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);
					printf("%3d d %d\t%d/%d/%d %d:%d:%d ..\n", 1, ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);
				}
				else
				{
					int p;
					p = dblock[now_d->db]->parent;

					for(i=0; i<512; ++i)
					{
						pd = &ip[i];
						if(now_d == pd)
							break;
					}

					printf("%3d d %d\t%d/%d/%d %d:%d:%d .\n", i+1, now_d->file_size, now_d->f_t.year, now_d->f_t.month, now_d->f_t.day, now_d->f_t.hour, now_d->f_t.minute, now_d->f_t.seconds);
					printf("%3d d %d\t%d/%d/%d %d:%d:%d ..\n", p+1, ip[p].file_size, ip[p].f_t.year, ip[p].f_t.month, ip[p].f_t.day, ip[p].f_t.hour, ip[p].f_t.minute, ip[p].f_t.seconds);
				}

				for(i=0; i<count1; ++i)
				{
					int ino;

					ino = 100*(num[i][0]-48) + 10*(num[i][1]-48) + (num[i][2]-48);

					printf("%3d ", ino+1);
					if(strcmp(ip[ino].file_type, "directory")==0)
						printf("d ");
					else
						printf("- ");

					printf("%d\t", ip[ino].file_size);
					printf("%d/%d/%d %d:%d:%d %s\n", ip[ino].f_t.year, ip[ino].f_t.month, ip[ino].f_t.day, ip[ino].f_t.hour, ip[ino].f_t.minute, ip[ino].f_t.seconds, name[i]);
				}
			}
			else if(words[3] == ' ')
			{
				for(i=4; words[i]!='\0'; ++i)
					words[i-4] = words[i];
				words[i-4] = '\0';

				char root[100];

				if(words[0] == '/')
				{
					remove_nowd(words);
					strcpy(root, words);
				}
				else
				{
					strcpy(root, mypwd);

					for(i=0; root[i]!='\0'; ++i)
						;

					if(root[i-1] != '/')
					{
						root[i] = '/';
						root[i+1] = '\0';
					}

					for(i=0; root[i]!='\0'; ++i)
						;

					for(j=0; words[j]!='\0'; ++j)
					{
						root[i] = words[j];
						i++;
					}
					root[i] = '\0';

					remove_nowd(root);
					strcpy(words, root);

				}

				if(words[0] == '\0')
				{
					words[0] = '/';
					words[1] = '\0';
					find = 1;
				}
				else if(words[0] == '/' && words[1] == '\0')
					find = 1;
				else
					find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

				if(find == 0 || find == 3)
					printf("Try input again\n");
				else
				{
					while(1)
					{
						stop = 0; 
						for(i=0; root[i]!='\0'; ++i)
							if(root[i] == '/')
								if(root[i+1] == '.')
									if(root[i+2] == '.')
									{
										stop = 1;
										break;
									}
						if(stop == 1)
						{
							for(j=i-1; j>0; --j)
								if(root[j] == '/')
									break;

							for(k=i+1; root[k]!='\0'; ++k)
								if(root[k] == '/')
									break;

							for(; root[k]!='\0'; ++k)
							{
								root[j] = root[k];
								j++;
							}
							root[j] = '\0';
						}
						else
							break;
					}

					if(strcmp(root, "/")==0)
					{
						find = 1;
						in = 0;
					}
					else
					{
						find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);

						for(i=0; i<4&&root[i]!='\0'; ++i)
							root[i] = root[i];
						root[i] = '\0';

						in = name_find(ip, root, pd);

						if(strcmp(ip[in].file_type, "directory")==0)
							find = 1;
						else
							find = 2;
					}

					if(find == 1)
					{
						stop = 0;
						tmp = dblock[ip[in].db];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								if(tmp->con.dc[i][0]=='\0')
								{
									stop = 1;
									break;
								}
								count++;
							}
							if(stop == 1)
								break;

							tmp = tmp->link;
						}

						char name[count][5];
						char num[count][4];

						stop = 0;
						k=0;
						tmp = dblock[ip[in].db];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								if(tmp->con.dc[i][0]=='\0')
								{
									stop = 1;
									break;
								}
								for(j=0; j<3; ++j)
									num[k][j] = tmp->con.dc[i][j];
								num[k][j] = '\0';

								for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
									name[k][j-3] = tmp->con.dc[i][j];
								name[k++][j-3] = '\0';
							}
							if(stop == 1)
								break;

							tmp = tmp->link;
						}

						count1 = count;

						for(i=count-1; i>0; --i)
						{
							for(j=0; j<count-1; ++j)
							{
								if(strcmp(name[j], name[j+1])>0)
								{
									strcpy(a, name[j]);
									strcpy(name[j], name[j+1]);
									strcpy(name[j+1], a);

									strcpy(n, num[j]);
									strcpy(num[j], num[j+1]);
									strcpy(num[j+1], n);
								}
							}
							count--;
						}

						if(in == 0)
						{
							printf("%3d d %d\t%d/%d/%d %d:%d:%d .\n",in+1, ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);
							printf("%3d d %d\t%d/%d/%d %d:%d:%d ..\n",in+1, ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);

							for(i=0; i<count1; ++i)
							{
								int ino;
								ino = 100*(num[i][0]-48) + 10*(num[i][1]-48) + (num[i][2]-48);

								printf("%3d ", ino+1);
								if(strcmp(ip[ino].file_type, "directory")==0)
									printf("d ");
								else
									printf("- ");

								printf("%d\t%d/%d/%d %d:%d:%d %s\n", ip[ino].file_size, ip[ino].f_t.year, ip[ino].f_t.month, ip[ino].f_t.day, ip[ino].f_t.hour, ip[ino].f_t.minute, ip[ino].f_t.seconds, name[i]);
							}
						}
						else
						{
							printf("%3d d %d\t%d/%d/%d %d:%d:%d .\n", in+1, ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds);
							int p;
							p = dblock[ip[in].db]->parent;
							printf("%3d d %d\t%d/%d/%d %d:%d:%d ..\n", p+1, ip[p].file_size, ip[p].f_t.year, ip[p].f_t.month, ip[p].f_t.day, ip[p].f_t.hour, ip[p].f_t.minute, ip[p].f_t.seconds);

							pd = &ip[in];
							for(i=0; i<count1; ++i)
							{
								int ino;
								ino = 100*(num[i][0]-48) + 10*(num[i][1]-48) + (num[i][2]-48);

								printf("%3d ", ino+1);
								if(strcmp(ip[ino].file_type, "directory")==0)
									printf("d ");
								else
									printf("- ");

								printf("%d\t%d/%d/%d %d:%d:%d %s\n", ip[ino].file_size, ip[ino].f_t.year, ip[ino].f_t.month, ip[ino].f_t.day, ip[ino].f_t.hour, ip[ino].f_t.minute, ip[ino].f_t.seconds, name[i]);
							}
						}
					}
					else
					{
						printf("%3d - %d\t%d/%d/%d %d:%d:%d %s\n", in+1, ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds, root);
					}

				}

			}
			else
				printf("Try input again\n");
		}
		else if(words[1] == 'l')
		{
			if(words[2] == '\0')
			{
				count = 0;

				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}
						count++;
					}

					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				char name[count][5];

				stop = 0;
				k=0;
				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}

						for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
							name[k][j-3] = tmp->con.dc[i][j];
						name[k++][j-3] = '\0';
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				count1 = count;

				for(i=count-1; i>0; --i)
				{
					for(j=0; j<count-1; ++j)
					{
						if(strcmp(name[j], name[j+1])>0)
						{
							strcpy(a, name[j]);
							strcpy(name[j], name[j+1]);
							strcpy(name[j+1], a);
						}
					}
					count--;
				}

				if(now_d->db == 0)
				{
					printf("d %d\t%d/%d/%d %d:%d:%d .\n", ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);
					printf("d %d\t%d/%d/%d %d:%d:%d ..\n", ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);
				}
				else
				{
					printf("d %d\t%d/%d/%d %d:%d:%d .\n", now_d->file_size, now_d->f_t.year, now_d->f_t.month, now_d->f_t.day, now_d->f_t.hour, now_d->f_t.minute, now_d->f_t.seconds);
					int p;
					p = dblock[now_d->db]->parent;
					printf("d %d\t%d/%d/%d %d:%d:%d ..\n", ip[p].file_size, ip[p].f_t.year, ip[p].f_t.month, ip[p].f_t.day, ip[p].f_t.hour, ip[p].f_t.minute, ip[p].f_t.seconds);
				}

				for(i=0; i<count1; ++i)
				{
					in = name_find(ip, name[i], now_d);

					if(strcmp(ip[in].file_type, "directory")==0)
						printf("d ");
					else
						printf("- ");

					printf("%d\t", ip[in].file_size);
					printf("%d/%d/%d %d:%d:%d ", ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds);

					printf("%s\n", name[i]);
				}
			}
			else if(words[2] == ' ')
			{
				for(i=3; words[i]!='\0'; ++i)
					words[i-3] = words[i];
				words[i-3] = '\0';
				//printf("words : %s\n", words);

				char root[100];

				if(words[0] == '/')
				{
					remove_nowd(words);
					strcpy(root, words);
				}
				else
				{
					strcpy(root, mypwd);

					for(i=0; root[i]!='\0'; ++i)
						;

					if(root[i-1] != '/')
					{
						root[i] = '/';
						root[i+1] = '\0';
					}

					for(i=0; root[i]!='\0'; ++i)
						;

					for(j=0; words[j]!='\0'; ++j)
					{
						root[i] = words[j];
						i++;
					}
					root[i] = '\0';

					remove_nowd(root);
					strcpy(words, root);

				}

				if(words[0] == '\0')
				{
					words[0] = '/';
					words[1] = '\0';
					find = 1;
				}
				else if(words[0] == '/' && words[1] == '\0')
					find = 1;
				else
					find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

				if(find == 0 || find == 3)
					printf("Try input again\n");
				else
				{
					while(1)
					{
						stop = 0; 
						for(i=0; root[i]!='\0'; ++i)
							if(root[i] == '/')
								if(root[i+1] == '.')
									if(root[i+2] == '.')
									{
										stop = 1;
										break;
									}
						if(stop == 1)
						{
							for(j=i-1; j>0; --j)
								if(root[j] == '/')
									break;

							for(k=i+1; root[k]!='\0'; ++k)
								if(root[k] == '/')
									break;

							for(; root[k]!='\0'; ++k)
							{
								root[j] = root[k];
								j++;
							}
							root[j] = '\0';
						}
						else
							break;
					}

					if(strcmp(root, "/")==0)
					{
						find = 1;
						in = 0;
					}
					else
					{
						find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);

						for(i=0; i<4&&root[i]!='\0'; ++i)
							root[i] = root[i];
						root[i] = '\0';

						in = name_find(ip, root, pd);

						if(strcmp(ip[in].file_type, "directory")==0)
							find = 1;
						else
							find = 2;
					}

					if(find == 1)
					{
						stop = 0;
						tmp = dblock[ip[in].db];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								if(tmp->con.dc[i][0]=='\0')
								{
									stop = 1;
									break;
								}
								count++;
							}
							if(stop == 1)
								break;

							tmp = tmp->link;
						}

						char name[count][5];

						stop = 0;
						k=0;
						tmp = dblock[ip[in].db];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								if(tmp->con.dc[i][0]=='\0')
								{
									stop = 1;
									break;
								}

								for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
									name[k][j-3] = tmp->con.dc[i][j];
								name[k++][j-3] = '\0';
							}
							if(stop == 1)
								break;

							tmp = tmp->link;
						}

						count1 = count;

						for(i=count-1; i>0; --i)
						{
							for(j=0; j<count-1; ++j)
							{
								if(strcmp(name[j], name[j+1])>0)
								{
									strcpy(a, name[j]);
									strcpy(name[j], name[j+1]);
									strcpy(name[j+1], a);
								}
							}
							count--;
						}

						if(in == 0)
						{
							printf("d %d\t%d/%d/%d %d:%d:%d .\n", ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);
							printf("d %d\t%d/%d/%d %d:%d:%d ..\n", ip[0].file_size, ip[0].f_t.year, ip[0].f_t.month, ip[0].f_t.day, ip[0].f_t.hour, ip[0].f_t.minute, ip[0].f_t.seconds);

							pd = &ip[in];
							for(i=0; i<count1; ++i)
							{
								in = name_find(ip, name[i], pd);

								if(strcmp(ip[in].file_type, "directory")==0)
									printf("d ");
								else
									printf("- ");

								printf("%d\t%d/%d/%d %d:%d:%d %s\n", ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds, name[i]);
							}
						}
						else
						{
							printf("d %d\t%d/%d/%d %d:%d:%d .\n", ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds);
							int p;
							p = dblock[ip[in].db]->parent;
							printf("d %d\t%d/%d/%d %d:%d:%d ..\n", ip[p].file_size, ip[p].f_t.year, ip[p].f_t.month, ip[p].f_t.day, ip[p].f_t.hour, ip[p].f_t.minute, ip[p].f_t.seconds);

							pd = &ip[in];
							for(i=0; i<count1; ++i)
							{
								in = name_find(ip, name[i], pd);

								if(strcmp(ip[in].file_type, "directory")==0)
									printf("d ");
								else
									printf("- ");

								printf("%d\t%d/%d/%d %d:%d:%d %s\n", ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds, name[i]);
							}
						}
					}
					else
					{
						printf("- %d\t%d/%d/%d %d:%d:%d %s\n", ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds, root);
					}

				}
			}
			else
				printf("Try input again\n");
		}
		else if(words[1] == 'i')
		{
			if(words[2] == '\0')
			{
				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}
						count++;
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				char name[count][5];
				char num[count][4];

				stop = 0;
				k=0;
				tmp = dblock[now_d->db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}

						for(j=0; j<3; ++j)
							num[k][j] = tmp->con.dc[i][j];
						num[k][j] = '\0';

						for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
							name[k][j-3] = tmp->con.dc[i][j];
						name[k++][j-3] = '\0';
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				count1 = count;

				for(i=count-1; i>0; --i)
				{
					for(j=0; j<count-1; ++j)
					{
						if(strcmp(name[j], name[j+1])>0)
						{
							strcpy(a, name[j]);
							strcpy(name[j], name[j+1]);
							strcpy(name[j+1], a);

							strcpy(n, num[j]);
							strcpy(num[j], num[j+1]);
							strcpy(num[j+1], n);
						}
					}
					count--;
				}

				if(now_d->db == 0)
				{
					printf("1	.\n");
					printf("1	..\n");
				}
				else
				{
					for(i=0; i<512; ++i)
					{
						pd = &ip[i];
						if(pd == now_d)
							break;
					}

					printf("%d	.\n", i+1);
					printf("%d	..\n", (dblock[ip[i].db]->parent)+1);
				}

				for(i=0; i<count1; ++i)
				{
					ino = 100*(num[i][0]-48) + 10*(num[i][1]-48) + (num[i][2]-48);
					printf("%d	%s\n", ino+1, name[i]);
				}
			}
			else if(words[2] == ' ')
			{
				for(i=3; words[i]!='\0'; ++i)
					words[i-3] = words[i];
				words[i-3] = '\0';

				char root[100];

				if(words[0] == '/')
				{
					remove_nowd(words);
					strcpy(root, words);
				}
				else
				{
					strcpy(root, mypwd);

					for(i=0; root[i]!='\0'; ++i)
						;

					if(root[i-1] != '/')
					{
						root[i] = '/';
						root[i+1] = '\0';
					}

					for(i=0; root[i]!='\0'; ++i)
						;

					for(j=0; words[j]!='\0'; ++j)
					{
						root[i] = words[j];
						i++;
					}
					root[i] = '\0';

					remove_nowd(root);
					strcpy(words, root);

				}

				if(words[0] == '\0')
				{
					words[0] = '/';
					words[1] = '\0';
					find = 1;
				}
				else if(words[0] == '/' && words[1] == '\0')
					find = 1;
				else
					find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

				if(find == 0 || find == 3)
					printf("Try input again\n");
				else
				{
					while(1)
					{
						stop = 0; 
						for(i=0; root[i]!='\0'; ++i)
							if(root[i] == '/')
								if(root[i+1] == '.')
									if(root[i+2] == '.')
									{
										stop = 1;
										break;
									}
						if(stop == 1)
						{
							for(j=i-1; j>0; --j)
								if(root[j] == '/')
									break;

							for(k=i+1; root[k]!='\0'; ++k)
								if(root[k] == '/')
									break;

							for(; root[k]!='\0'; ++k)
							{
								root[j] = root[k];
								j++;
							}
							root[j] = '\0';
						}
						else
							break;
					}

					if(strcmp(root, "/")==0)
					{
						find = 1;
						in = 0;
					}
					else
					{
						find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);

						for(i=0; i<4&&root[i]!='\0'; ++i)
							root[i] = root[i];
						root[i] = '\0';

						in = name_find(ip, root, pd);

						if(strcmp(ip[in].file_type, "directory")==0)
							find = 1;
						else
							find = 2;
					}

					if(find == 1)
					{
						stop = 0;
						tmp = dblock[ip[in].db];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								if(tmp->con.dc[i][0]=='\0')
								{
									stop = 1;
									break;
								}
								count++;
							}
							if(stop == 1)
								break;

							tmp = tmp->link;
						}

						char name[count][5];
						char num[count][4];

						stop = 0;
						k=0;
						tmp = dblock[ip[in].db];
						while(tmp!=NULL)
						{
							for(i=0; i<24; ++i)
							{
								if(tmp->con.dc[i][0]=='\0')
								{
									stop = 1;
									break;
								}

								for(j=0; j<3; ++j)
									num[k][j] = tmp->con.dc[i][j];
								num[k][j] = '\0';

								for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
									name[k][j-3] = tmp->con.dc[i][j];
								name[k++][j-3] = '\0';
							}
							if(stop == 1)
								break;

							tmp = tmp->link;
						}

						count1 = count;

						for(i=count-1; i>0; --i)
						{
							for(j=0; j<count-1; ++j)
							{
								if(strcmp(name[j], name[j+1])>0)
								{
									strcpy(a, name[j]);
									strcpy(name[j], name[j+1]);
									strcpy(name[j+1], a);

									strcpy(n, num[j]);
									strcpy(num[j], num[j+1]);
									strcpy(num[j+1], n);
								}
							}
							count--;
						}

						if(in == 0)
						{
							printf("%d	.\n", in+1);
							printf("%d	..\n", in+1);

							for(i=0; i<count1; ++i)
							{
								int ino;
								ino = 100*(num[i][0]-48) + 10*(num[i][1]-48) + (num[i][2]-48);
								printf("%d	%s\n", ino+1, name[i]);
							}
						}
						else
						{
							pd = &ip[in];
							printf("%d	.\n", in+1);
							printf("%d	..\n", (dblock[pd->db]->parent)+1);

							for(i=0; i<count1; ++i)
							{
								int ino;
								ino = 100*(num[i][0]-48) + 10*(num[i][1]-48) + (num[i][2]-48);
								printf("%d	%s\n", ino+1, name[i]);
							}
						}
					}
					else
					{
						printf("- %d\t%d/%d/%d %d:%d:%d %s\n", ip[in].file_size, ip[in].f_t.year, ip[in].f_t.month, ip[in].f_t.day, ip[in].f_t.hour, ip[in].f_t.minute, ip[in].f_t.seconds, root);
					}

				}
			}
			else
				printf("Try input again\n");
		}
		else
			printf("Try input again\n");

	}
	else
	{
		char root[100];

		if(words[0] == '/')
		{
			remove_nowd(words);
			strcpy(root, words);
		}
		else
		{
			strcpy(root, mypwd);

			for(i=0; root[i]!='\0'; ++i)
				;

			if(root[i-1]!='/')
			{
				root[i] = '/';
				root[i+1] = '\0';
			}

			for(i=0; root[i]!='\0'; ++i)
				;

			for(j=0; words[j]!='\0'; ++j)
			{
				root[i] = words[j];
				i++;
			}
			root[i] = '\0';

			remove_nowd(root);
			strcpy(words, root);
		}

		if(words[0] == '\0')
		{
			words[0] = '/';
			words[1] = '\0';
			find = 1;
		}
		else if(words[0] == '/' && words[1] == '\0')
			find = 1;
		else
			find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

		if(find == 0||find==3)
			printf("Try input again\n");
		else
		{
			while(1)
			{
				stop = 0;
				for(i=0; root[i]!='\0'; ++i)
					if(root[i] == '/')
						if(root[i+1] == '.')
							if(root[i+2] == '.')
							{
								stop = 1;
								break;
							}

				if(stop == 1)
				{
					for(j=i-1; j>0; --j)
						if(root[j] == '/')
							break;

					for(k=i+1; root[k]!='\0'; ++k)
						if(root[k] == '/')
							break;

					for(; root[k]!='\0'; ++k)
					{
						root[j] = root[k];
						j++;
					}
					root[j] = '\0';
				}
				else
					break;
			}

			if(strcmp(root, "/") == 0)
			{
				find = 1;
				in = 0;
			}
			else
			{
				find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);

				for(i=0; i<4&&root[i]!='\0'; ++i)
					root[i] = root[i];
				root[i] = '\0';

				in = name_find(ip, root, pd);

				if(strcmp(ip[in].file_type, "directory") == 0)
					find = 1;
				else
					find = 2;
			}

			if(find == 1)
			{
				stop = 0;
				tmp = dblock[ip[in].db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}
						count++;
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				char name[count][5];
				k=0;
				stop = 0;

				tmp = dblock[ip[in].db];
				while(tmp!=NULL)
				{
					for(i=0; i<24; ++i)
					{
						if(tmp->con.dc[i][0]=='\0')
						{
							stop = 1;
							break;
						}

						for(j=3; j<7&&tmp->con.dc[i][j]!='\0'; ++j)
							name[k][j-3] = tmp->con.dc[i][j];
						name[k++][j-3] = '\0';
					}
					if(stop == 1)
						break;

					tmp = tmp->link;
				}

				count1 = count;

				for(i=count-1; i>0; --i)
				{
					for(j=0; j<count-1; ++j)
						if(strcmp(name[j], name[j+1])>0)
						{
							strcpy(a, name[j]);
							strcpy(name[j], name[j+1]);
							strcpy(name[j+1], a);
						}
					count--;
				}

				printf(".\n");
				printf("..\n");

				for(i=0; i<count1; ++i)
					printf("%s\n", name[i]);
			}
			else
				printf("%s\n", root);
		}
	}
}
void func_myshowfile(inode *ip, char words[], inode * now_d)
{
	int i, j, k, l, stop = 0;
	int count, in;
	int count1 = 0, count2 = 0;
	char name[5];
	data *tmp;
	unsigned long long a, b1=0, b2=0;

	if(words[0]==' '||words[0]=='\0')
		printf("Try input again\n");
	else
	{
		for(i=0; words[i]!='\0'; ++i)
			if(words[i]=='-')
			{
				stop = 1;
				break;
			}
		if(stop == 1)
			printf("Try input again\n");
		else
		{

			for(i=0; words[i]!=' '; ++i)
				count1++;

			for(i=i+1; words[i]!=' '; ++i)
				count2++;

			char num1[count1+1], num2[count2+1];

			for(i=0; i<count1; ++i)
				num1[i] = words[i];
			num1[i] = '\0';

			for(k=0; k<count2; ++k)
				num2[k] = words[k+i+1];
			num2[k] = '\0';

			for(l=k+i+2; words[l]!='\0'; ++l)
				name[l-(k+i+2)] = words[l];
			name[l-(k+i+2)] = '\0';

			//printf("name : %s\n", name);
			for(i=0; num1[i]!='\0'; ++i)
			{
				a = num1[i]-48;
				for(j=1; j<count1; ++j)
					a *= 10;
				count1--;
				b1 += a;
			}

			for(i=0; num2[i]!='\0'; ++i)
			{
				a = num2[i]-48;
				for(j=1; j<count2; ++j)
					a *= 10;
				count2--;
				b2 += a;
			}

			unsigned long long byte;

			byte = b2 - b1 + 1;

			if(byte <= 0)
				printf("Try input again\n");
			else
			{
				count = 1;

				for(i=0; i<4&&name[i]!='\0'; ++i)
					name[i] = name[i];
				name[i] = '\0';

				in = name_find(ip, name, now_d);

				if(in == 0)
					printf("myshowfile : failed to show : File not exist\n");
				else
				{
					if(strcmp(ip[in].file_type, "directory")==0)
						printf("myshowfile : failed to show : '%s' is directory\n", name);
					else
					{
						if(b2>ip[in].file_size)
							printf("Try input again\n");
						else
						{
							char show[byte+1];
							int s = 0;

							tmp = dblock[ip[in].db];

							while(tmp!=NULL)
							{
								for(i=0; tmp->con.fc[i]!='\0'; ++i)
								{
									if(count>=b1&&count<=b2)
									{
										show[s] = tmp->con.fc[i];
										s++;
									}
									count++;
								}
								tmp = tmp->link;
							}
							show[s] = '\0';

							printf("%s", show);
						}
					}
				}
			}
		}
	}
}
int func_mycd(inode *ip, char words[], inode *now_d, inode **now_dp, int now_d_num)
{
	inode *pd;
	inode *tmp;
	char root[100];
	int i, j, k, stop=0, in;
	int find = 0;
	int nd, pnd;

	if(words[0] == '\0')
	{
		mypwd[0] = '/';
		mypwd[1] = '\0';
		*now_dp = &ip[0];
		return 0;
	}
	else if(words[0] == '/' && words[1] == '\0')
	{
		mypwd[0] = '/';
		mypwd[1] = '\0';
		*now_dp = &ip[0];
		return 0;
	}
	else
	{
		for(i=0; words[i]!='\0'; ++i)
			;

		if(words[i-1] == '/')
			words[i-1] = '\0';

		for(i=0; words[i]!='\0'; ++i)
			if(words[i]==' ')
			{
				stop = 1;
				break;
			}

		if(stop == 1)
			printf("Try input again\n");
		else
		{
			if(words[0] == '/') // 절대경로
			{
				for(i=0; words[i]!='\0'; ++i)
					root[i] = words[i];
				root[i] = '\0';

				remove_nowd(root);
				strcpy(words, root);
			}
			else
			{
				strcpy(root, mypwd);

				if(strcmp(root, "/")==0)
					;
				else
				{
					for(i=0; root[i]!='\0'; ++i)
						;

					root[i] = '/';
					root[i+1] = '\0';
				}

				for(i=0; root[i]!='\0'; ++i)
					;

				for(j=0; words[j]!='\0'; ++j)
				{
					root[i] = words[j];
					i++;
				}
				root[i] = '\0';

				remove_nowd(root);

				strcpy(words, root);

			}

			if(words[0] == '\0')
			{
				words[0] == '/';
				words[1] = '\0';
				find = 1;
			}
			else
				find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

			if(find == 0)
				printf("Try input again\n");
			else if(find == 1)
			{
				while(1)
				{
					find = 0;
					for(i=0; root[i]!='\0'; ++i)
						if(root[i]=='/')
							if(root[i+1]=='.')
								if(root[i+2]=='.')
								{
									find = 1;
									break;
								}
					if(find == 1)
					{
						for(j=i-1; j>0; --j)
							if(root[j]=='/')
								break;

						for(k=i+1; root[k]!='\0'; ++k)
							if(root[k]=='/')
								break;

						if(root[k] == '/')
						{
							for(; root[k]!='\0'; ++k)
							{
								root[j] = root[k];
								j++;
							}
							root[j] = '\0';
						}
						else
							root[j] = '\0';
					}
					else
						break;
				}

				if(strcmp(root, "/")==0)
				{
					strcpy(mypwd, root);
					*now_dp = pd;
					return 0;
				}
				else
				{
					int check=0;
					int count=0;

					while(1)
					{
						for(i=0; root[i]!='\0'; ++i)
						{
							if(root[i]=='/')
								count = 0;
							else
							{
								count++;

								if(count==5)
									break;
							}
						}

						if(root[i]=='\0')
							break;

						if(count==5)
							for(j=i; root[j]!='\0'; ++j)
								if(root[j]=='/')
									break;

						for(; root[j]!='\0'; ++j)
						{
							root[i] = root[j];
							i++;
						}
						root[i] = '\0';

					}

					strcpy(mypwd, root);

					find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);
					for(i=0; i<4&&root[i]!='\0'; ++i)
						root[i] = root[i];
					root[i] = '\0';

					in = name_find(ip, root, pd);

					*now_dp = &ip[in];
					return in;
				}
			}
			else if(find == 2)
				printf("Try input again\n");
			else
				printf("Try input again\n");
		}
	}
}
void func_mycpto(inode *ip, char words[], inode *now_d)
{
	char f1[5], f2[5];
	int i, j, in;
	data *tmp;
	FILE *ofp;


	if(words[0] == ' '||words[0]=='\0')
		printf("Try input again1\n");
	else
	{
		for(i=0; i<4&&words[i]!=' '; ++i)
			f1[i] = words[i];
		f1[i] = '\0';

		for(i=0; words[i]!=' '; ++i)
			;

		for(j=0, i=i+1; words[i]!='\0'; ++i)
		{
			words[j] = words[i];
			j++;
		}
		words[j] = '\0';

		if(words[0] == ' ')
			printf("Try input again2\n");
		else
		{
			for(i=0; words[i]!='\0'; ++i)
				f2[i] = words[i];
			f2[i] = '\0';

			in = name_find(ip, f1, now_d);

			if(in == 0)
				printf("mycpto : '%s' 파일을 찾을 수 없습니다 : File does not exists\n", f1);
			else
			{
				if(ip[in].file_type=="directory")
					printf("mycpto : '%s'는 디렉토리 입니다.\n", f1);
				else
				{
					ofp = fopen(f2, "w");

					tmp = dblock[ip[in].db];
					while(tmp!=NULL)
					{
						for(i=0; tmp->con.fc[i]!='\0'; ++i)
							fprintf(ofp, "%c", tmp->con.fc[i]);
						tmp = tmp->link;
					}

					fclose(ofp);
				}
			}
		}
	}
}
void func_mytouch(char words[], super *sp, inode *ip, inode *now_d, struct tm *t, int now_d_num)
{
	int i, j, k, in, dn, stop=0;
	data *tmp;
	int find;
	inode *pd;
	int nd, pnd;

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
				char root[100];

				if(words[0]!='/')
				{
					strcpy(root, mypwd);

					for(i=0; root[i]!='\0'; ++i)
						;

					if(root[i-1]!='/')
					{
						root[i] = '/';
						root[i+1] = '\0';
					}

					for(i=0; root[i]!='\0'; ++i)
						;

					for(j=0; words[j]!='\0'; ++j)
					{
						root[i] = words[j];
						i++;
					}
					root[i] = '\0';

					strcpy(words, root);

				}
				else
					strcpy(root, words);

				remove_nowd(root);
				remove_nowd(words);

				if(words[0] == '\0')
				{
					words[0] = '/';
					words[1] = '\0';
					strcpy(root, words);
				}

				if(words[0] == '/' && words[1] == '\0')
					find = 1;

				else
					find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

				if(find == 0)
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
					dblock[dn]->parent = pnd;
					dblock[dn]->d_num = dn;

					if(dblock[pd->db]->con.dc[0][0]=='\0')
						dblock[pd->db]->down = dblock[dn];
					else
					{
						tmp = dblock[pd->db]->down;
						while(tmp->next!=NULL)
						{
							tmp = tmp->next;
						}
						tmp->next = dblock[dn];
					}

					name_write(in, pd, sp, words);
				}
				else if(find==1||find==2)
				{
					if(root[0] == '/'&&root[1] == '\0')
						in = 0;
					else
					{
						while(1)
						{
							stop = 0;
							for(i=0; root[i]!='\0'; ++i)
								if(root[i]=='/')
									if(root[i+1]=='.')
										if(root[i+2]=='.')
										{
											stop = 1;
											break;
										}
							if(stop == 1)
							{
								for(j=i-1; j>0; --j)
									if(root[j] == '/')
										break;

								for(k=i+1; root[k]!='\0'; ++k)
									if(root[k] == '/')
										break;

								for(; root[k]!='\0'; ++k)
								{
									root[j] = root[k];
									j++;
								}
								root[j] = '\0';
							}
							else
								break;
						}

						if(root[0] == '/'&&root[1]=='\0')
							in = 0;

						else
						{
							find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);

							for(i=0; i<4&&root[i]!='\0'; ++i)
								root[i] = root[i];
							root[i] = '\0';

							in = name_find(ip, root, pd);
						}



					}

					ip[in].f_t.year = 1900+t->tm_year; 
					ip[in].f_t.month = 1+t->tm_mon;
					ip[in].f_t.day = t->tm_mday;
					ip[in].f_t.hour = t->tm_hour;
					ip[in].f_t.minute = t->tm_min;
					ip[in].f_t.seconds = t->tm_sec;
				}
				else
					printf("Try input again\n");
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
									fread(&(dblock[dn]->con.fc[i]), sizeof(char), 1, ofp);
								dblock[dn]->con.fc[i] = '\0';

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
									fread(&(dblock[dn]->con.fc[i]), sizeof(char), 1, ofp);
								dblock[dn]->con.fc[i] = '\0';

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
									while(1)
									{
										if(fread(&(dblock[dn]->con.fc[i]), sizeof(char), 1, ofp)==0)
										{
											dblock[dn]->con.fc[i] = '\0';
											break;
										}
										else if(i==128)
										{
											dblock[dn]->con.fc[i] = '\0';
											fseek(ofp, -1, SEEK_CUR);
											break;
										}
										else

											i++;
									}

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
								fclose(ofp);

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

								// 첫번째 데이터 블록 생성.
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
									fread(&(dblock[dn]->con.fc[i]), sizeof(char), 1, ofp);
								dblock[dn]->con.fc[i] = '\0';

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
										fread(&(dblock[dn]->con.fc[i]), sizeof(char), 1, ofp);
									dblock[dn]->con.fc[i] = '\0';

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

								int j = 1;
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
										if(dcount+1!=102)
											dblock[ip[in].dib]->con.ind[dcount+1] = 0;

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
									while(1)
									{
										if(fread(&(dblock[dn]->con.fc[i]), sizeof(char), 1, ofp)==0)
										{
											dblock[dn]->con.fc[i] = '\0';
											break;
										}
										else if(i==128)
										{
											dblock[dn]->con.fc[i] = '\0';
											fseek(ofp, -1, SEEK_CUR);
											break;
										}
										i++;
									}

									tmp = dblock[ip[in].db];
									while(tmp->link!=NULL)
									{
										tmp = tmp->link;
									}
									tmp->link = dblock[dn];

									dblock[s_dn]->con.ind[scount] = dn;
									if(scount+1!=102)
										dblock[s_dn]->con.ind[scount+1] = 0;

									scount++;
									count++;

								}
								fclose(ofp);
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
	int j, k, a, find, stop = 0;
	data *tmp;
	inode *pd;
	int pnd;
	int nd;

	if(words[0] == '\0')
		printf("mymkdir : missing operand\n");
	else if(words[0] == '/' && words[1] == '\0')
		printf("mymkdir : '/' 디렉토리를 만들 수 없습니다 : Directory exists\n");
	else if(words[0] == '.' && words[1] == '\0')
		printf("mymkdir : '.' 디렉토리를 만들 수 없습니다 : Directory exists\n");
	else
	{
		for(i=0; words[i]!='\0'; ++i)
			if(words[i]==' ')
			{
				stop = 1;
				break;
			}

		if(stop == 1)
			printf("Try input again\n");
		else
		{
			remove_nowd(words);

			find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);  // pd : 부모 주소, nd 맨 마지막꺼 아이노드 번호, pnd : 부모 아이노드번호.

			if(find == 0)
			{
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
				dblock[dn]->parent = pnd;
				dblock[dn]->type = 'd';
				dblock[dn]->d_num = dn;

				// 부모, 형제 디렉터리와 연결
				if(dblock[pd->db]->con.dc[0][0]=='\0')
					dblock[pd->db]->down = dblock[dn];
				else
				{
					tmp = dblock[pd->db]->down;
					while(tmp->next != NULL)
					{
						tmp = tmp->next;
					}
					tmp->next = dblock[dn];
				}

				name_write(in, pd, sp, words);
			}
			else if(find == 1||find == 2)
				printf("mymkdir : '%s' 디렉토리를 만들 수 없습니다 : File exists\n", words);
			else
				printf("Try input again\n");
		}
	}
}
void func_myrm(super *sp, char words[], inode *ip, inode *now_d, int now_d_num)
{
	int find, dn, in;
	int pnd, nd;
	int i;
	inode *pd;
	data *tmp;
	data *tmp1;
	data *tmp2;

	remove_nowd(words);
	find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

	if(find == 0)
	{
		printf("myrm : failed to remove '%s' : No such file or directory\n", words);
	}
	else
	{
		if(find == 1)
		{
			printf("myrm : failed to remove '%s' : Not a file\n", words);
		}
		else if(find == 2)
		{
			for(i=0; i<4&&words[i]!='\0'; ++i)
				words[i] = words[i];
			words[i] = '\0';

			in = name_find(ip, words, pd);
			dn = ip[in].db;

			name_remove(pd, sp, dn, in);

			if(ip[in].sib!=-1)
			{
				free(dblock[ip[in].sib]);
				dblock[ip[in].sib] = NULL;
				bit_data(sp, ip[in].sib);
			}

			int temp;

			if(ip[in].dib!=-1)
			{
				for(i=0; i<102; ++i)
				{
					if(dblock[ip[in].dib]->con.ind[i]!=0)
					{
						free(dblock[dblock[ip[in].dib]->con.ind[i]]);
						dblock[dblock[ip[in].dib]->con.ind[i]] = NULL;
						temp = dblock[ip[in].dib]->con.ind[i];
						bit_data(sp, temp);
					}
					else
						break;
				}

				free(dblock[ip[in].dib]);
				dblock[ip[in].dib] = NULL;
				bit_data(sp, ip[in].dib);
			}

			dn = ip[in].db;

			int size;
			size = ip[in].file_size;

			ip[in].file_type = "";
			ip[in].f_t.year = 0;
			ip[in].f_t.month = 0;
			ip[in].f_t.day = 0;
			ip[in].f_t.hour =  0;
			ip[in].f_t.minute = 0;
			ip[in].f_t.seconds = 0;
			ip[in].file_size = 0;
			ip[in].db = 0;
			ip[in].sib = 0;
			ip[in].dib = 0;

			if((dblock[pd->db]->down) == dblock[dn])
			{
				dblock[pd->db]->down = dblock[dn]->next;
			}
			else
			{
				tmp = dblock[pd->db]->down;
				while(tmp!=NULL)
				{
					if((tmp->next)==dblock[dn])
						break;
					tmp = tmp->next;
				}
				tmp->next = dblock[dn]->next;
			}

			if(size <= 128)
				;
			else
			{
				tmp1 = dblock[dn];
				tmp2 = dblock[dn]->link;
				while(1)
				{
					tmp1 = dblock[dn];
					tmp2 = dblock[dn]->link;
					while(tmp2->link!=NULL)
					{
						tmp2 = tmp2->link;
						tmp1 = tmp1->link;
					}

					temp = tmp2->d_num;
					bit_data(sp, temp);
					free(dblock[temp]);
					dblock[temp] = NULL;
					tmp1->link = NULL;
					if(tmp1 == dblock[dn])
						break;
				}
			}

			free(dblock[dn]);
			dblock[dn] = NULL;

			bit_inode(sp, in);
			bit_data(sp, dn);
		}
		else
			printf("myrm : failed to remove : 잘못된 경로입니다.\n");
	}
}
void func_myrmdir(super *sp, char words[], inode *ip, inode *now_d, int now_d_num)
{
	int find, in, dn, stop = 0;
	int i, j, k;
	data *tmp;
	data *tmp1;
	data *tmp2;
	inode *pd;
	int nd, pnd;

	remove_nowd(words);
	find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd); //자기자신의 주소값 pd에 리턴.

	if(find == 0)   // 현재 디렉토리에 없음.
	{
		printf("myrmdir : failed to remove '%s' : No such file or directory\n", words);
	}
	else
	{
		if(find == 2)
		{
			printf("myrmdir : failed to remove '%s' : Not a Directory\n", words);
		}
		else if(find == 1) // 디렉토리임.
		{
			for(i=0; i<4&&words[i]!='\0'; ++i)
				words[i] = words[i];
			words[i] = '\0';

			in = name_find(ip, words, pd); 
			dn = ip[in].db;  // dn : 지우려는 디렉토리 첫 데이터블록 번호.

			if(dblock[dn]->con.dc[0][0] != '\0')
			{
				printf("myrmdir : failed to remove '%s' : Directory not empty\n", words);
			}
			else
			{
				name_remove(pd, sp, dn, in);
				// 아이노드에서 정보 삭제.

				int size, temp;

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

				if((dblock[pd->db]->down) == dblock[dn])
				{
					dblock[pd->db]->down = dblock[dn]->next;
				}
				else
				{
					tmp = dblock[pd->db]->down;
					while(tmp!=NULL)
					{
						if((tmp->next)==dblock[dn])
							break;
						tmp = tmp->next;
					}
					tmp->next = dblock[dn]->next;
				}

				free(dblock[dn]);
				dblock[dn] = NULL;

				bit_inode(sp, in);
				bit_data(sp, dn);
			}
		}
		else
			printf("myrmdir : failed to remove : 잘못된 경로입니다.\n");
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
void func_mytree(char words[], inode *ip, inode *now_d, int now_d_num)
{
	int i;
	int stop = 0, j, find = 0, k, in;
	int pnd, nd;
	inode *pd;

	if(words[0] == '\0')
	{
		print_tree(ip, 0, dblock[ip[now_d_num].db]);
	}
	else
	{
		//지정한 경로의 data block 주소값을 tmp에 지정한다.
		char root[100];

		if(words[0] == '/')
		{
			remove_nowd(words);
			strcpy(root, words);
		}
		else
		{
			strcpy(root, mypwd);

			for(i=0; root[i]!='\0'; ++i)
				;

			if(root[i-1]!='/')
			{
				root[i] = '/';
				root[i+1] = '\0';
			}

			strcat(root, words);

			remove_nowd(root);
			strcpy(words, root);
		}

		if(words[0] == '\0')
		{
			words[0] = '/';
			words[1] = '\0';
			find = 1;
		}
		else if(words[0] == '/' && words[1] == '\0')
			find = 1;
		else
			find = find_root(ip, words, now_d, now_d_num, &pnd, &pd, &nd);

		if(find == 0||find==3)
			printf("Try input again\n");
		else
		{
			while(1)
			{
				stop = 0;
				for(i=0; root[i]!='\0'; ++i)
					if(root[i] == '/')
						if(root[i+1] == '.')
							if(root[i+2] == '.')
							{
								stop = 1;
								break;
							}

				if(stop == 1)
				{
					for(j=i-1; j>0; --j)
						if(root[j] == '/')
							break;

					for(k=i+1; root[k]!='\0'; ++k)
						if(root[k] == '/')
							break;

					for(; root[k]!='\0'; ++k)
					{
						root[j] = root[k];
						j++;
					}
					root[j] = '\0';
				}
				else
					break;
			}

			if(strcmp(root, "/") == 0)
			{
				find = 1;
				in = 0;
			}
			else
			{
				find = find_root(ip, root, now_d, now_d_num, &pnd, &pd, &nd);

				for(i=0; i<4&&root[i]!='\0'; ++i)
					root[i] = root[i];
				root[i] = '\0';

				in = name_find(ip, root, pd);

				if(strcmp(ip[in].file_type, "directory") == 0)
					find = 1;
				else
					find = 2;
			}

			if(find == 1)
			{
				stop = 0;
				print_tree(ip, 0, dblock[ip[in].db]);
			}
		}
	}
}
void print_tree(inode *ip, int n, data *now)
{
	int i, j;
	char name[5];

	if(now == NULL)
		return;//가리키는 포인터가 NULL인경우 종료

	if(n != 0)//처음이 아닌 경우 -와 *를 출력
	{
		for(i=0; i<n-1; i++)
			printf("---");
		printf("--* ");
	}
	else
		if(now->d_num == 0)//처음이 /인 경우
		{
			printf("/\n");

			print_tree(ip, n+1, now->down);//다음 자식노드를 탐색하여 출력

			return;
		}
		else//처음이 /가 아닌 경우 이름을 찾아 출력
		{
			for(i=0; dblock[ip[now->parent].db]->con.dc[i][0]!='\0'; i++)
				if(now->d_num == (100*(dblock[ip[now->parent].db]->con.dc[i][0]-48)+10*(dblock[ip[now->parent].db]->con.dc[i][1]-48)+(dblock[ip[now->parent].db]->con.dc[i][2]-48)))
					break;

			for(j=0; j<4; j++)
				name[j] = dblock[ip[now->parent].db]->con.dc[i][3+j];
			name[4] = '\0';
			printf("%s\n", name);

			print_tree(ip, n+1, now->down);//다음 자식노드를 탐색하여 출력(형제노드는 출력하지 않는다.)

			return;
		}

	for(i=0; dblock[ip[now->parent].db]->con.dc[i][0]!='\0'; i++)
		if(now->d_num == (100*(dblock[ip[now->parent].db]->con.dc[i][0]-48)+10*(dblock[ip[now->parent].db]->con.dc[i][1]-48)+(dblock[ip[now->parent].db]->con.dc[i][2]-48)))
			break;

	for(j=0; j<4; j++)
		name[j] = dblock[ip[now->parent].db]->con.dc[i][3+j];
	name[4] = '\0';
	
	printf("%s\n", name);

	print_tree(ip, n+1, now->down);//다음 자식노드를 먼저 탐색하여 출력
	print_tree(ip, n, now->next);//다음 형제노드를 탐색하여 출력

	return;
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
				if(ip[in-1].file_type==""||ip[in-1].file_type==NULL)
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
	FILE *ofp;
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
							printf("%d	", in+1);
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
					for(i=0; i<102&&dblock[dn]->con.ind[i]!=0; ++i)
						printf("%d ", dblock[dn]->con.ind[i]+1);
					printf("\n");
				}
				else
				{
					printf("%s", dblock[dn]->con.fc);

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
int find_root(inode *ip, char words[], inode *now_d, int now_d_num, int *pnd, inode **pd, int *nd)
{
	int i, j, stop=0, find=0;
	int yet;
	char dir_name[5];

	find = now_d_num;

	for(i=0; words[i]!='\0'; ++i)
		;
	if(words[i-1] == '/')
		words[i-1] = '\0';
	else
		;

	if(words[0]!='/')
	{
		while(1)
		{
			stop = 0;
			for(i=0; words[i]!='\0'; ++i)
				if(words[i]=='/')
				{
					stop = 1;
					break;
				}


			if(stop == 1)
			{
				for(i=0; i<4&&words[i]!='/'; ++i)
					dir_name[i] = words[i];
				dir_name[i] = '\0';

				if(strcmp(dir_name, "..")==0)
				{
					if(dblock[now_d->db]->parent == -1)
						return 3;
					else
						now_d = &ip[dblock[now_d->db]->parent];
				}
				else
				{
					yet = find;
					find = name_find(ip, dir_name, now_d);

					if(find == 0)
						return 3;  // return 3이면 error
					else
					{
						if(ip[find].file_type=="directory")
							now_d = &ip[find];
						else
							return 3;
					}
				}

				for(i=0; words[i]!='/'; ++i)
					;

				for(i=i+1, j=i; words[i]!='\0'; ++i)
					words[i-j] = words[i];
				words[i-j] = '\0';
			}
			else
			{
				for(i=0; i<4&&words[i]!='\0'; ++i)
					dir_name[i] = words[i];
				dir_name[i] = '\0';

				if(strcmp(dir_name, "..")==0)
				{
					if(dblock[now_d->db]->parent == -1)
						return 3;
					else
					{
						*pnd = yet;
						*pd = &ip[dblock[now_d->db]->parent];
						return 1;
					}
				}
				else
				{
					yet = find;
					find = name_find(ip, dir_name, now_d);

					if(find==0)
					{
						*pnd = yet;
						*pd = now_d;
						return 0;
					}
					else
					{
						if(ip[find].file_type=="directory")
						{
							*pnd = yet;
							*pd = now_d;
							*nd = find;
							return 1;
						}
						else
						{
							*pnd = yet;
							*pd = now_d;
							*nd = find;
							return 2;
						}
					}
				}
			}
		}
	}
	else
	{
		now_d = &ip[0];
		for(i=0; words[i]!='\0'; ++i)
			words[i] = words[i+1];
		words[i] = '\0';

		while(1)
		{
			stop = 0;
			for(i=0; words[i]!='\0'; ++i)
				if(words[i]=='/')
				{
					stop = 1;
					break;
				}


			if(stop == 1)
			{
				for(i=0; i<4&&words[i]!='/'; ++i)
					dir_name[i] = words[i];
				dir_name[i] = '\0';

				if(strcmp(dir_name, "..")==0)
				{
					if(dblock[now_d->db]->parent == -1)
						return 3;
					else
						now_d = &ip[dblock[now_d->db]->parent];
				}
				else
				{
					yet = find;
					find = name_find(ip, dir_name, now_d);

					if(find==0)
						return 3; // return 3이면 error
					else
					{
						if(ip[find].file_type=="directory")
							now_d = &ip[find];
						else
							return 3;
					}
				}

				for(i=0; words[i]!='/'; ++i)
					;

				for(i=i+1, j=i; words[i]!='\0'; ++i)
					words[i-j] = words[i];
				words[i-j] = '\0';
			}
			else
			{
				for(i=0; i<4&&words[i]!='\0'; ++i)
					dir_name[i] = words[i];
				dir_name[i] = '\0';

				if(strcmp(dir_name, "..")==0)
				{
					if(dblock[now_d->db]->parent==-1)
						return 3;
					else
					{
						*pnd = yet;
						*pd = &ip[dblock[now_d->db]->parent];
						return 1;
					}
				}
				else
				{
					yet = find;
					find = name_find(ip, dir_name, now_d);

					if(find == 0)
					{
						*pnd = yet;
						*pd = now_d;
						return 0;
					}
					else
					{
						if(ip[find].file_type == "directory")
						{
							*pnd = yet;
							*pd = now_d;
							*nd = find;
							return 1;
						}
						else
						{
							*pnd = yet;
							*pd = now_d;
							*nd = find;
							return 2;
						}
					}
				}
			}
		}
	}
}
void name_remove(inode *pd, super *sp, int dn, int in)
{
	data *tmp, *tmp2;
	int i, j, k, stop = 0;

	tmp = dblock[pd->db];
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

	tmp2 = dblock[pd->db];
	while(tmp2->link!=NULL)
	{
		tmp2 = tmp2 -> link;	
	}

	for(j=0; tmp2->con.dc[j][0]!='\0'; ++j)
		;

	for(k=0; k<7; ++k)
		tmp->con.dc[i][k] = tmp2->con.dc[j-1][k];

	tmp2->con.dc[j-1][0] = '\0';
	tmp2->con.dc[j-1][3] = '\0';

	// 맨 마지막 데이터블록이 비었는지 확인.
	if(pd->sib!=-1)
	{
		if(tmp2->con.dc[0][0]=='\0')
		{
			tmp = dblock[pd->db];
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
				if(dblock[pd->sib]->con.ind[i]==dn)
				{
					dblock[pd->sib]->con.ind[i] = 0;
					break;
				}


			if(dblock[pd->sib]->con.ind[0]==0) // 싱글 다 비어있음.
			{
				free(dblock[pd->sib]);
				bit_data(sp, pd->sib);
				dblock[pd->sib] = NULL;
				pd->sib = -1;
			}
			else
				;
		}
		else // 비어있지 않음.
			;

	}
	else  // single indirect가 없음.
		;
}
void remove_nowd(char words[])
{
	int i, j, stop=0;

	while(1)
	{
		stop = 0;
		for(i=0; words[i]!='\0'; ++i)
			if(words[i] == '/')
				if(words[i+1] == '.')
					if(words[i+2] == '/'||words[i+2] == '\0')
					{
						stop = 1;
						break;
					}

		if(stop == 1)
		{
			for(j=i+2; words[j]!='\0'; ++j)
			{
				words[i] = words[j];
				i++;
			}
			words[i] = '\0';
		}
		else
		{
			if(words[0] == '.'&&words[1] == '/')
				stop = 2;

			if(stop == 2)
			{
				for(j=1; words[j]!='\0'; ++j)
					words[j-1] = words[j];
				words[j-1] = '\0';
			}
			else
			{
				if(strcmp(words, ".") == 0)
					words[0] = '\0';

				else
					break;
			}

		}
	}
}
void final_func(super *sp, inode *ip)
{
	int i;
	int temp;

	FILE *ofp;
	ofp = fopen("myfs", "wb");

	for(i=0; i<64; ++i)
		fwrite(&(sp->inode_use[i]), sizeof(sp->inode_use[i]), 1, ofp);

	for(i=0; i<128; ++i)
		fwrite(&(sp->datablock_use[i]), sizeof(sp->datablock_use[i]), 1, ofp);

	for(i=0; i<512; ++i)
	{
		if(ip[i].file_type == "" || ip[i].file_type == NULL)
			;
		else
			fwirte(&ip[i], sizeof(ip[i]), 1, ofp);
	}

	for(i=0; i<1024; ++i)
	{
		if(dblock[i] == NULL)
			;
		else
		{
			if(dblock[i]->link == NULL)
				dblock[i]->link = (data *)0;
			else
			{
				temp = dblock[i]->link->d_num;
				dblock[i]->link = (data *)temp;
			}

			if(dblock[i]->down == NULL)
				dblock[i]->dwon = (data *)0;
			else
			{
				temp = dblock[i]->down->d_num;

			fwrite(&dblock[i], sizeof(dblock[i]), 1, ofp);

	}
}
