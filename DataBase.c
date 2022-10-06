#include <stdio.h>
#include <io.h>
#include <string.h>
#include <direct.h>
#define N 4  //字段类型种类数
//定义字段类型结构体
typedef struct FieldTyperange
{
	char fieldType[50];  //字段类型
	int minLength;		//字段最小长度
	int maxLength;		//字段最大长度
	int isFixed;         //字段长度是否固定
} fieldType;

//定义字段结构体，保存表的字段信息
typedef struct Table
{
	char tableName[50]; //表名
	char fieldName[50];  //字段名
	char fieldType[50];  //字段类型
	char fieldLength[10];//字段长度
	char constraint[50];//约束
} table;

//定义结构体保存表名及相关信息
typedef struct TableName
{
	char tableName[50]; //表名
	char fieldCount[50];  //字段数
	char location[50];  //保存位置
} tableName;

//约束
typedef struct Constraint
{
	char Constraint[50];  //约束类型
} constraint;

//初始化相关变量
fieldType field[N];
table tab[100];
constraint constr[20];
char workLocation[100];
FILE *fp_TS = NULL;
FILE *fp_DS = NULL;

//函数声明
void initializeFieldType();
void showMenu();
void replaceTo(char * ch);
void operateMenu();
void CreateTable();
void addRecord(char *tableName,char * tableLocation);
void showRecord(char *tableName,char * tableLocation);
void findLine(FILE *fp,char* ch1, char* ch2);
void operateRecord();
int judgeFieldType(char * fieldType);
int judgeConstraint(char * constraint);
void readTableStr(char *tableName);
void JieQu(char* ch1, int count , char* ch2);
void QueryTable();
void QueryTableInDatabase();
int TableIsReappear(char *tableName);
int isDate(char * ch);
int isInt(char * ch);
int isInputPass(char * input, char * type);
/*实验4*/
int isAccordPrimarykey(char * value,char *tableName,char * tableLocation);
int isAccordConstraint(char * value,char * constraint,char *tableName,char * tableLocation);

int main()
{
	int ch;
	strcpy(workLocation,getcwd(NULL, NULL));
	if(access("structure",0) !=  -1)
	{
		//文件夹存在
		//chdir("/structure");
		if(access("./structure/表结构信息.txt",0) !=  -1)
		{
			//文件存在
			fp_TS = fopen("./structure/表结构信息.txt","a+");
		}else
		{
			//文件不存在
			fp_TS = fopen("./structure/表结构信息.txt","w+");
		}

		if(access("./structure/数据库表信息.txt",0) !=  -1)
		{
			//文件存在
			fp_DS = fopen("./structure/数据库表信息.txt","a+");
		}else
		{
			//文件不存在
			fp_DS = fopen("./structure/数据库表信息.txt","w+");
		}
	}else
	{
		//文件夹不存在
		mkdir("structure");
		fp_TS = fopen("./structure/表结构信息.txt","w+");
		fp_DS = fopen("./structure/数据库表信息.txt","w+");
	}
	
	if(access("Date",0) !=  -1)
	{
	}else
	{
		//文件夹不存在
		mkdir("Date");
	}

	
	initializeFieldType();
	while(1)
	{
		showMenu();
		scanf("%d",&ch);
		system("cls");
		fp_TS = fopen("./structure/表结构信息.txt","a+");
		fp_DS = fopen("./structure/数据库表信息.txt","a+");
		switch(ch)
		{
			case 1:
				CreateTable();
				break;
			case 2:
				QueryTable();
				break;
			case 3:
				QueryTableInDatabase();
				break;
			case 4:
				operateRecord();
				break;
			case 0:
				fclose(fp_TS);
				exit();
				break;
			default :
				printf("没有此选项！\n");
				break;
		}
		fclose(fp_TS);
		fclose(fp_DS);
	}
	fclose(fp_TS);
	fclose(fp_DS);
}

//主菜单
void showMenu()
{
	printf("\n-----------------------\n");
	printf("   1、定义表结构\n");  
	printf("   2、查询表结构信息\n");
	printf("   3、显示所有表\n");
	printf("   4、操作表\n");
	printf("   0、退出\n");
	printf("-----------------------\n");
	printf("输入操作：\n");
}

//对表中数据进行的操作
void operateMenu()
{
	printf("\n---------------------\n");
	printf("   1、添加记录\n");  
	printf("   2、打印记录\n");
	printf("   3、修改记录\n");
	printf("   4、删除记录\n");
	printf("   0、返回\n");
	printf("-----------------------\n");
	printf("输入操作：\n");
}

//查询当前有那些表,并输出
void QueryTableInDatabase()
{
	//操作数据库表信息.txt
	char ch,c[100];
	char temp[20];
	fp_DS = fopen("./structure/数据库表信息.txt","a+");
	printf("-----------------------\n");
	printf("          查看表       \n");
	printf("-----------------------\n");
	if(fgetc(fp_DS) == EOF)
	{
		printf("没有任何表！\n");
	}else{
		while(ch != EOF)//循环输出文件的每一行
		{
			//按行读取文本,将每一行数据截取后保存到结构体中
			fseek(fp_DS,-1,SEEK_CUR);
			fscanf(fp_DS,"%[^\n]", c);
			JieQu(c, 1 , temp);
			printf("	%s\n",temp);
			if(fgetc(fp_DS) == '\n')//遇到换行符，换行
			ch = fgetc(fp_DS);		
		}
	}
	printf("-----------------------\n");
	fclose(fp_DS);
}

//通过表名查询表，判断指定表是否已经存在，1表示存在，0表示不存在
int TableIsReappear(char *tableName)
{
	//操作数据库表信息.txt
	char ch,c[100];
	char temp[20];
	fp_DS = fopen("./structure/数据库表信息.txt","a+");//有访问文件的操作时最好用前打开，用后关闭，防止前后函数连续访问同一个文件时出错
	if(fgetc(fp_DS) == EOF)
	{
		//没有任何表！
	}else{
		while(ch != EOF)//循环输出文件的每一行
		{
			//按行读取文本,将每一行数据截取后保存到结构体中
			fseek(fp_DS,-1,SEEK_CUR);
			fscanf(fp_DS,"%[^\n]", c);
			JieQu(c, 1 , temp);
			
			if(strcmp(tableName,temp) == 0)
			{
				fclose(fp_DS);
				return 1;
			}
			
			if(fgetc(fp_DS) == '\n')//遇到换行符，换行
			ch = fgetc(fp_DS);		
		}
	}
	fclose(fp_DS);
	return 0;
}



//删除表

//修改表结构

//对表中的数据进行操作
void operateRecord()
{
	char tableName[50];
	char tableNameCopy[50];   //findLine()方法好像会改变原有参数的值，使用备份
	int ch;
	char line[50];
	char tableLocation[50];
	//FILE * fp = NULL;
	QueryTableInDatabase();
	printf("请输入你要操作的表名\n");
	scanf("%s",tableName);
	strcpy(tableNameCopy,tableName);
	if(!TableIsReappear(tableName))
	{
		printf("请输入正确的表名\n");
	}
	else
	{
		readTableStr(tableName);
		findLine("./structure/数据库表信息.txt",tableNameCopy, line);
		JieQu(line, 3 , tableLocation);
		printf("保存位置：[%s]\n",tableLocation);
		while(1)
		{
			operateMenu();
			scanf("%d",&ch);
			system("cls");
		//	fp = fopen(tableLocation,"a+");
			switch(ch)
			{
				case 1:
					addRecord(tableName,tableLocation);
					break;
				case 2:
					showRecord(tableName,tableLocation);
					break;
				case 3:
					printf("尚未开放，敬请期待！\n");
					break;
				case 4:
					printf("尚未开放，敬请期待！\n");
					break;
				case 0:
				//	fclose(fp);
					return;
					break;
				default :
					printf("没有此选项！\n");
					break;
			}
			//fclose(fp);
		}
	}
	//fclose(fp);
}

//添加记录
void addRecord(char *tableName,char * tableLocation)
{
	FILE * fp = NULL;
	char line[100];
	char inputLine[200];
	char temp[50];
	char n[10];
	int fNum = 0;
	int recNum = 0;
	int i = 0;
	int j = 0;
	char tableNameCopy[50];   //findLine()方法好像会改变原有参数的值，使用备份
	strcpy(tableNameCopy,tableName);
	findLine("./structure/数据库表信息.txt",tableNameCopy, line); //tableLocation
	//通过表名，查询到表结构（有几个字段）
	//1.查询表有几个字段，将每个字段的信息存入结构体
	JieQu(line, 2 , n);
	fNum = atoi(n);

	printf("请输入输入记录的条数\n");
	scanf("%d",&recNum);
	system("cls");
	printf("%s\n",tableLocation);
	fp = fopen(tableLocation,"a+");
	for(i = 0;i < recNum; i++)
	{
		strcpy(inputLine," ");
		printf("输入第%d条记录：\n",i+1);
		for(j = 0; j < fNum; j++)
		{
			printf("输入%s[%s(%s)]：\n",tab[j].fieldName,tab[j].fieldType,tab[j].fieldLength);
			scanf("%s",temp);
			while(isInputPass(temp, tab[j].fieldType) != 1){
				//输入不合法；
				if(strcmp(tab[j].fieldType,"varchar") == 0)
				{
					if(strlen(temp)<=atoi(tab[j].fieldLength))
					{
						break;
					}
				}
				printf("输入不合法！请重新输入%s[%s(%s)]：\n",tab[j].fieldName,tab[j].fieldType,tab[j].fieldLength);
				scanf("%s",temp);
			}
			
			//判断输入是否符合约束
			while(isAccordConstraint(&temp,tab[j].constraint,tableName,tableLocation) != 1)
			{
				//不符合约束；
				printf("输入不符合约束！请重新输入%s[%s(%s)--%s]：\n",tab[j].fieldName,tab[j].fieldType,tab[j].fieldLength,tab[j].constraint);
				scanf("%s",temp);
			}

			fprintf(fp,"%s ",temp);
			if(j != fNum-1)
			{
				fprintf(fp," ");
			}			
		}
		printf("-----------------------\n");
		fprintf(fp,"\n");
	}
	fclose(fp);
}

//删除记录

//查询记录
void showRecord(char *tableName,char * tableLocation)
{
	FILE * fp = NULL;
	char * p,*s;
	char line[100];
	char outputLine[200];
	char temp[50];
	char n[10];
	char ch,c[100];
	int fNum = 0;  //有几个字段
	int recNum = 0;
	int i = 0,j = 0;
	char tableNameCopy[50];   //findLine()方法好像会改变原有参数的值，使用备份
	
	strcpy(tableNameCopy,tableName);
	findLine("./structure/数据库表信息.txt",tableName, line); //tableLocation
	//通过表名，查询到表结构（有几个字段）
	//1.查询表有几个字段，将每个字段的信息存入结构体
	JieQu(line, 2 , n);
	fNum = atoi(n);
	printf("%s\n",tableLocation);
	printf("---------------------------------------------------------------------------------------\n");
	fp = fopen(tableLocation,"a+");
	if(fgetc(fp) == EOF)
	{
		printf("没有此记录\n");
	}else{
		//printf("---%s\n",tab[0].fieldName);
		for(j = 0; j < fNum; j++)
		{
			printf("   %s\t\t",tab[j].fieldName);
			//strcat(outputLine, tab[i].fieldName); 
			//strcat(outputLine, '\t'); 
		}
		printf("\n");
	//	printf("---%s\n",tab[0].fieldName);
		while(ch != EOF)//循环输出文件的每一行
		{
			//按行读取文本,将每一行数据截取后保存到结构体中
			fseek(fp,-1,SEEK_CUR);
			fscanf(fp,"%[^\n]", c);
			p = outputLine;
			for(i = 0; i < fNum; i++)
			{
				s = temp;
				JieQu(c, i+1 , &temp);
				
				while(*s != '\0')
				{
					*p++ = *s++;
				}
				*p++ = '\t'; 
				*p++ = '\t';
				//*p++ = '\t';
			}
			*p = '\0';
			printf("%s\n",outputLine);
			if(fgetc(fp) == '\n')//遇到换行符，换行
			ch = fgetc(fp);		
		}
	}
	printf("---------------------------------------------------------------------------------------\n");
	fclose(fp);
}

//修改记录

//输入表结构的函数（建表）
void CreateTable()
{
	int i = 0;
	char i2[2];
	int j = 0;
	int k = 0;
	char absLocation[100];
	strcpy(absLocation,workLocation);
	printf("请输入表名：\n");
	scanf("%s",tab[0].tableName);
	if(TableIsReappear(&tab[0].tableName))
	{
		printf("该表已存在！\n");
		return -1;
	}
	printf("定义的表结构的字段个数\n");
	scanf("%d",&i);
	strcpy(i2,itoa(i));
	for(; i >= 1 ; i--, j++)
	{
		strcpy(tab[j].tableName,tab[0].tableName);
		printf("请输入第%d个字段名：\n",j+1);
		scanf("%s",tab[j].fieldName);
		printf("请输入第%d个字段类型[%s %s %s]：\n",j+1,field[0].fieldType,field[1].fieldType,field[2].fieldType);
		scanf("%s",tab[j].fieldType);
		while((k = judgeFieldType(&tab[j].fieldType)) == -1)
		{
			printf("非法输入，请重新输入第%d个字段类型：\n",j+1);
			scanf("%s",tab[j].fieldType);
		}
		
		if(field[k].isFixed == 1)
		{
			strcpy(tab[j].fieldLength,itoa(field[k].minLength));
		}
		else
		{
			printf("请输入第%d个字段长度[%s(%d-%d)]：\n",j+1,field[k].fieldType,field[k].minLength,field[k].maxLength);
			scanf("%s",&tab[j].fieldLength);
			while((atoi(tab[j].fieldLength) < field[k].minLength) || (atoi(tab[j].fieldLength) > field[k].maxLength))
			{
				printf("字段长度不符合要求，请重新输入第%d个字段长度：\n",j+1);
				scanf("%s",&tab[j].fieldLength);
			}
		}
		printf("为第%d个字段添加约束[%s %s %s %s]（输入0为不添加任何约束）：\n",j+1,constr[1].Constraint,constr[2].Constraint,constr[3].Constraint,constr[4].Constraint);
		scanf("%s",&tab[j].constraint);
		while((k = judgeConstraint(&tab[j].constraint) == -1))
		{
			printf("非法输入，请重新输入第%d个字段约束[%s %s %s %s]：\n",j+1,constr[1].Constraint,constr[2].Constraint,constr[3].Constraint,constr[4].Constraint);
			scanf("%s",&tab[j].constraint);
		}
		fprintf(fp_TS,"%s %s %s %s %s\n",tab[j].tableName,tab[j].fieldName,tab[j].fieldType,tab[j].fieldLength,tab[j].constraint);
	}
	strcat(absLocation, "\\Date\\");
	strcat(absLocation, tab[0].tableName);
	strcat(absLocation, ".txt");
	replaceTo(absLocation);
//	creat(absLocation,0);
	fopen(absLocation,"w+");
	printf("-----------------------------------\n");
	printf("创建成功！\n文件保存位置：[%s]\n",absLocation);
	fp_DS = fopen("./structure/数据库表信息.txt","a+");
	fprintf(fp_DS,"%s %s %s\n",tab[0].tableName,i2,absLocation);
	fclose(fp_DS);
}

//查询指定表的结构信息
void QueryTable()
{
	char tableName[50];
	char ch,c[100];
	int i = 0,j = 0;
	QueryTableInDatabase();
	printf("请输入您要查询的表名：\n");
	scanf("%s",tableName);
	if(TableIsReappear(tableName) == 0)
	{
		printf("--------------------------------------------------------------------\n");
		printf("	该表不存在！\n");
		printf("--------------------------------------------------------------------\n");
		return -1;
	}
	while(ch != EOF)//循环输出文件的每一行
	{
		//按行读取文本,将每一行数据截取后保存到结构体中
		fseek(fp_TS,-1,SEEK_CUR);
		fscanf(fp_TS,"%[^\n]", c);
		JieQu(c, 1 , &tab[i].tableName);
		
		if(strcmp(tableName,tab[i].tableName) == 0)
		{
			JieQu(c, 2 , &tab[i].fieldName);
			JieQu(c, 3 , &tab[i].fieldType);
			JieQu(c, 4 , &tab[i].fieldLength);
			JieQu(c, 5 , &tab[i].constraint);
			i++;
		}
		
		if(fgetc(fp_TS) == '\n')//遇到换行符，换行
		ch = fgetc(fp_TS);		
	}
	printf("------------------------------------------------------------------------------------\n");
	printf("字段名\t\t\t数据类型\t\t字段长度\t\t约束类型\n");
	for(j = 0; j < i; j++)
	{
		printf("%s\t\t\t%s\t\t\t%s\t\t\t%s\n",tab[j].fieldName,tab[j].fieldType,tab[j].fieldLength,tab[j].constraint);
	}
	printf("------------------------------------------------------------------------------------\n");
}

//判断输入的字段类型是否符合要求,若符合要求，返回该字段的下标，若不符合要求，返回-1
int judgeFieldType(char * fieldType)
{
	int i = 0;	
	for(i = 0; i <= N; i++ )
	{
		if(strcmp(fieldType,field[i].fieldType) == 0)
		{
			return i;
		}
	}
	return -1;
}

//初始化字段类型
void initializeFieldType()
{
	strcpy(field[0].fieldType,"varchar");
	field[0].minLength = 0;
	field[0].maxLength = 5000;
	field[0].isFixed = 0;

	strcpy(field[1].fieldType,"int");
	field[1].minLength = 4;
	field[1].maxLength = 4;
	field[1].isFixed = 1;

	strcpy(field[2].fieldType,"date");
	field[2].minLength = 8;
	field[2].maxLength = 8;
	field[2].isFixed = 1;

	strcpy(constr[0].Constraint,"0");
	strcpy(constr[1].Constraint,"PRIMARYKEY");
	strcpy(constr[2].Constraint,"NOTNULL");
	strcpy(constr[3].Constraint,"CHECK(gender)");
	strcpy(constr[4].Constraint,"CHECK(date)");

}

//传入表名将该表的结构存入结构体里
void readTableStr(char *tableName)
{
	char ch,c[100];
	int i = 0;
	if(!TableIsReappear(tableName))
	{
		printf("--------------------------------------------------------------------\n");
		printf("	该表不存在\n");
		printf("--------------------------------------------------------------------\n");
		return -1;
	}
	while(ch != EOF)//循环输出文件的每一行
	{
		//按行读取文本,将每一行数据截取后保存到结构体中
		fseek(fp_TS,-1,SEEK_CUR);
		fscanf(fp_TS,"%[^\n]", c);
		JieQu(c, 1 , &tab[i].tableName);
		
		if(strcmp(tableName,tab[i].tableName) == 0)
		{
			JieQu(c, 2 , &tab[i].fieldName);
			JieQu(c, 3 , &tab[i].fieldType);
			JieQu(c, 4 , &tab[i].fieldLength);
			JieQu(c, 5 , &tab[i].constraint);
			i++;
		}
		
		if(fgetc(fp_TS) == '\n')//遇到换行符，换行
		ch = fgetc(fp_TS);		
	}
}

/*
//使judgeFieldType(char * fieldType)返回字段类型结构体数组对应的数组下标，以下方法就不需要实现了
//判断输入的字段类型长度是否固定，若固定返回字段长度，若不固定，返回-1
int isFixedOf()
{
}

//字段类型长度不固定时，返回字段长度最小值
int minLengthOf()
{
}

//字段类型长度不固定时，返回字段长度最大值
int maxLengthOf()
{
}
*/
//截取ch1第count个数据，赋值给ch2
//学生表 学号 string 20
void JieQu(char* ch1, int count , char* ch2)
{
	int i = 0;
	const char s[2] = " ";
    char *token;
	char copch1[100];
	strcpy(copch1,ch1);
	/* 获取第一个子字符串 */
	token = strtok(copch1, s);
	i++;
	/* 继续获取其他的子字符串 */
	
	while( token != NULL ) 
	{
		if(i == count)
			break;
		token = strtok(NULL,s);
		i++;
	}
	strcpy(ch2,token);
}
//通过第一个数据项(该项必须唯一)，找到指定文件的一行数据并将改行数据赋值给ch2，传递参数时，打开文件，执行结束后该函数会自动关闭文件
//传参时请传入备份！！！
void findLine(char *fileLocation,char* ch1, char* ch2)
{
	FILE *fp = NULL;
	char ch,c[100];
	char temp[50];
	char ch1Temp[50];
	strcpy(ch1Temp,ch1);
//	printf(".............................%s\n",ch1);
	if(TableIsReappear(ch1Temp) != 1)
	{
		printf("该项不存在！\n");
		return -1;
	}
	//printf("--------------------------------%s\n",ch1);
	fp = fopen(fileLocation,"a+");
	if(fgetc(fp) == EOF)
	{
		printf("表内容为空！\n");
	}else{
		while(ch != EOF)//循环输出文件的每一行
		{
			
			//按行读取文本,将每一行数据截取后保存到结构体中
			fseek(fp,-1,SEEK_CUR);
			fscanf(fp,"%[^\n]", c);
			JieQu(c, 1 , temp);
			
			if(strcmp(ch1Temp,temp) == 0)
			{
				//printf("找到啦8888888888888888888\n");
				strcpy(ch2,c);
				//printf("--------------------------------%s\n",ch1);
			}
			
			if(fgetc(fp) == '\n')//遇到换行符，换行
			ch = fgetc(fp);		
		}
	}
//	strcpy(ch1,ch1Temp);//ch1会有变化？
//	printf("--------------------------------%s\n",ch1);
//	printf("ch1Temp--------------------------------%s\n",ch1Temp);
	fclose(fp);
}

//将路径中的\转换为/防止在程序中转义字符的影响
void replaceTo(char * ch)
{
	char * f = NULL;
	f = ch;
	while(*f)
	{
		if(*f == '\\')
			*f = '/';
		f++;
	}
}


//传入输入的字符串，判断是否为int型，基于atoi(name)和itoa(num,ch2,10)函数，要#include <stdlib.h>
int isInt(char * ch)
{
	int num;
	int lenth1;
	int lenth2;
	char temp[50];

	lenth1 = strlen(ch);
	num = atoi(ch);
	itoa(num,temp,10);
	lenth2 = strlen(temp);

	if(lenth1 == lenth2)
	{
		return 1;
	}else if(lenth1-lenth2 == 1){
		return -1;
	}else{
		return 0;
	}
}

//传入一个字符串判断是否符合要求（日期格式20000819），基于atoi(name)和isInt(ch)函数，要#include <stdlib.h>
int isDate(char * ch)
{
	//1900年，是平年；2000年是闰年； 2008年是闰年
	char temp[5];
	char * p = NULL;
	char * s = NULL;
	int year = 0, mounth = 0, day = 0;
	int i = 0;
	if((isInt(ch) == 1) && (strlen(ch) == 8))
	{
		p = ch;
		s = temp;
		for(i = 0; i < 4; i++)
		{
			*s = *p;
			s++;
			p++;
		}
		year = atoi(temp);
		s = temp;
		for(i = 0; i < 2; i++)
		{
			*s = *p;
			s++;
			p++;
		}
		*s = '\0';
		mounth = atoi(temp);
		s = temp;
		for(i = 0; i < 2; i++)
		{
			*s = *p;
			s++;
			p++;
		}
		*s = '\0';
		day = atoi(temp);
		if(year < 1900 || year > 2022 || mounth < 1 || mounth > 12 || day < 1 || day > 31)
		{//
			return 0;
		}else{
			if((mounth == 4 || mounth == 6 || mounth == 9 || mounth == 11) && (day > 30))
			{
				//4，6，9，11月有30天
				return 0;
			}
			if(((year%4 != 0) || ((year%100 == 0) && (year%400 != 0))) && (mounth == 2) && (day > 28))
			{//平年2月28天
				return 0;
			}else if(((year%400 == 0) || ((year%4 == 0) && (year%100 != 0))) && (mounth == 2) && (day > 29))
			{//闰年2月29天
				return 0;
			}
		}
		return 1;
	}else{
		//printf("不是日期型\n");
		return 0;
	}
}

//传入输入的数据及应该输入什么类型，判断输入是否合法
int isInputPass(char * input, char * type)
{
	if(strcmp("int",type) == 0)
	{
		return isInt(input);
	}
	else if(strcmp("date",type) == 0)
	{
		return isDate(input);
	}
	else if(strcmp("varchar",type) == 0)
	{
		return 0;
	}
	else{
		printf("类型不存在，请重新输入\n");
		return 0;
	}
}

/*实验4*/
//判断输入的约束类型是否符合要求，返回该类型的下标，若不符合要求，返回-1
int judgeConstraint(char * constraint)
{
	int i = 0;	
	for(i = 0; i <= 5; i++ )
	{
		if(strcmp(constraint,constr[i].Constraint) == 0)
		{
			return i;
		}
	}
	return -1;
}

//判断输入是否符合约束
int isAccordConstraint(char * value,char * constraint,char *tableName,char * tableLocation)
{
	if(strcmp("0",constraint) == 0)
	{
		return 1;
	}
	else if(strcmp("PRIMARYKEY",constraint) == 0)
	{
		return isAccordPrimarykey(value,tableName,tableLocation);
	}
	else if(strcmp("NOTNULL",constraint) == 0)
	{
		if(strcmp("null",value) != 0)
			return 1;
	}
	else if(strcmp("CHECK(gender)",constraint) == 0)
	{
		if(strcmp("男",value) == 0 || strcmp("女",value) == 0)
			return 1;
	}
	else if(strcmp("CHECK(date)",constraint) == 0)
	{
		return isDate(value);
	}
	return 0;
}
//判断是否符合PRIMARYKEY约束
int isAccordPrimarykey(char * value,char *tableName,char * tableLocation)
{
	FILE * fp = NULL;
	char * p,*s;
	char line[100];
	char outputLine[200];
	char temp[50];
	char n[10];
	char ch,c[100];
	int fNum = 0;  //有几个字段
	int recNum = 0;
	int i = 0,j = 0;
	char tableNameCopy[50];   //findLine()方法好像会改变原有参数的值，使用备份
	
	strcpy(tableNameCopy,tableName);
	findLine("./structure/数据库表信息.txt",tableName, line); //tableLocation
	//通过表名，查询到表结构（有几个字段）
	//1.查询表有几个字段，将每个字段的信息存入结构体
	JieQu(line, 2 , n);
	fNum = atoi(n);
	fp = fopen(tableLocation,"a+");
	if(fgetc(fp) == EOF)
	{
		fclose(fp);
		return 1;
	}else{
		while(ch != EOF)//循环输出文件的每一行
		{
			fseek(fp,-1,SEEK_CUR);
			fscanf(fp,"%[^\n]", c);
			JieQu(c, 1, n);
			if(strcmp(n,value) == 0)
			{
				fclose(fp);
				return 0;
			}
			if(fgetc(fp) == '\n')//遇到换行符，换行
			ch = fgetc(fp);		
		}
	}
	fclose(fp);
	return 1;
}