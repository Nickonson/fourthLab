/* �ணࠬ�� ��� ����஢�� ᮮ�饭��, ������஢����� � ������� ������ 
�������. �ᯮ���� ��ॡ�� �� ᫮����. ������������ BC�5.02 Win32 only.
��� ��७�� �� gcc ����室��� ��९���� �� �㭪樨, ࠡ���騥 � �����窮� �
ࠡ��� � ᫮��६, ⠬ ��-� �� ᮢᥬ ⠪.
���������� ������⢥��� ��窨 �����!!!
�����: ���⮫�� ���祭���
	����� �������
	���⪮� ��������
	��ਦ��� ���३
	��ਭ �����
	������譮�� ����
	������ॢ ����⠭⨭
�����-������᪨� ���㤠��⢥��� ��孨�᪨� ���������
��䥤� ���� ��㯯� 1088/2
1999�.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include "vigener.h"
#include <share.h>
#include <alloc.h>
#include <mem.h>
/*===========================================================================*/

#define SIZE_OF_INPUT_ARRAY 3000
#define SIZE_OF_OUT_ARRAY 500
#define ALPHABET_LENGTH 32                              //����� ��䠢��.

/*===========================================================================*/
typedef struct queue
{
    char *data;
    struct queue *p;
} queue;

/*----------------------�������� ��६����--------------------------------*/

int    KeyLength;    //����� ���祢��� ᫮��.
FILE   *debug;       //�᫨ ��।�����, �뢮����� �� �஬������ १�����.
//���ᨢ ���� ������ ���᪨� �㪢 ������� ������쭮 ��� ������ float rus[32]

char *mem_err="�������筮 �����!\n";
long items;
queue *top=NULL;
queue *bottom=NULL;
/*===========================================================================*/

/*===========================================================================*/

void main(int argc, char *argv[])
{
    char  *   InString,                                //����஢���� ⥪��.
          *   OutString,                               //�����஢���� ⥪��.
          *   ReplaceMatrix,                           //����� ����⠭����.
          *   KeyWord;                                 //���祢�� ᫮��.
    int   **  Groups,                                  //����� ��㯯.

          *   Sdvigs;                                  //���ᨢ ᤢ���� ����� ��㯯���.

    float *** Pklr,
          *** PkXiYj;
    int ***ConvertedPklr;

    printf("�����-������᪨� ���㤠��⢥��� ��孨�᪨� ���������\n\
��䥤� ���� ��㯯� 1088/2, 1999 �.\n\
����஢騪 ⥪�⮢, ����஢����� �� ������� �������, ���. 1.0\n\
(�) ���⮫�� ���祭���, ����� �������, ���⪮� ��������, ��ਦ��� ���३,\n\
��ਭ �����, ������譮�� ����, ������ॢ ����⠭⨭, 1999-2000\n");	

    if(argc<3)
    {
        printf("�ᯮ�짮�����: VIGINER �室���_䠩� ��室���_䠩� [���७��_����]\n");
        return;
    }
    if(argc>=4 && (debug=fopen(argv[3],"a")))
    {
        printf("����७�� ���� �����뢠���� � 䠩�: %s\n",argv[3]);
    }

    InString = tdpReadInputFile(argv[1]);

    KeyLength = andreyKeyWordLength(InString);

    Groups = tdpGroupsFromFile (argv[1]);

    PkXiYj = tolyaPkXiYj(Groups);

    Pklr = vovaPklr(PkXiYj);

    ConvertedPklr=ConvertPklr(Pklr);

    if(debug)
    {
        fprintf(debug, "����� ConvertedPklr:\n");
        tdpShow3DMatrix(ConvertedPklr,KeyLength,KeyLength,ALPHABET_LENGTH);
    }

    //Keyword search using dictionary attack.

    enumerate(ConvertedPklr);

    if((KeyWord=(char *)calloc(KeyLength+1,sizeof(char)))==NULL)
    {
      printf(mem_err);
      exit(1);
    }

    printf("������ ���室�饥 ���祢�� ᫮��(�᫨ ���������� ����� ���祢��\n᫮�� ���஡�� ����� ��㡮��� ��� ����� ��ப�� ����): ");
    fflush(stdin);
    fscanf(stdin, "%s", KeyWord);
    //fgets(KeyWord,KeyLength+1,stdin);
    if(!KeyWord[0]) exit(1);

    if(debug) fprintf(debug, "\n\n���祢�� ᫮��: %s\n\n\n", KeyWord);

    Sdvigs=word2sdvig(KeyWord);

    //Uncomment this 3 lines for another keyword search method.

    //Keyword search using text statictics and visual control.
    //Sdvigs = tdpSdvigTable(Pklr);

    //KeyWord = tdpKeyWord(Sdvigs);

    //tdpCorrectSdvigTable(Sdvigs, KeyWord);

    ReplaceMatrix = tdpReplaceMatrix3(tdpPXiYjMatrix(Groups, Sdvigs));

    OutString = tdpOutToArray(InString, ReplaceMatrix, Sdvigs);

    ReplaceMatrix = tdpCorrectReplaceMatrix(InString, OutString, ReplaceMatrix, Sdvigs, KeyWord);

    tdpOutToFile(argv[1], argv[2], ReplaceMatrix, Sdvigs);

    printf("��⮢�!!!");

    free(KeyWord);

    if(debug)
    {
        fprintf(debug, "\n�����蠥� �믮������.\n");
        fclose(debug);
    }
}

/*-------------------------�⥭�� ��室���� 䠩�� � ���ᨢ-------------------*/

char * tdpReadInputFile(char * FileName)
{
    char *InputArray, ch;
    long NewLength = 0;

    FILE *InputFile;

    printf("����㦠�� ⥪�� �� 䠩�� %s...",FileName);

    if((InputFile = fopen(FileName, "rb")) == NULL)
    {
        perror(FileName);
        exit(1);
    }

    if ((InputArray = (char*)calloc((filelength(fileno(InputFile)) + 1), sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    while(((ch = getc(InputFile)) != EOF) /*&& (NewLength < SIZE_OF_INPUT_ARRAY)*/)
    {
        if((ch >= '�') && (ch <= '�'))
        {
            InputArray[NewLength++] = ch;
        }
    }

    InputArray = (char*)realloc(InputArray, (NewLength + 1));

    fclose(InputFile);

    printf(" ��⮢�.\n\n");

    if(debug)
    {
        fprintf(debug,"\n��ࠡ��뢠�� 䠩� %s\n%s\n\n",FileName,InputArray);
    }

    return(InputArray);
}

/*-----------------------���᫥��� ����� ���祢��� ᫮��--------------------*/

int andreyKeyWordLength(char *Text)
{
   float SrIS,IS;
   int TextLength=strlen(Text);
   int *KeyLength;
   int i,j,k,Cislo;
   int Matr[33];
   int count = 0;
   printf("����塞 ����� ���祢��� ᫮��...\n");
   if(debug) fprintf(debug,"����塞 ����� ���祢��� ᫮��:\n");
   /*�뤥����� ����� ��� ���ᨢ.*/
   if ((KeyLength = (int*)calloc(20, sizeof(int))) == NULL)
   {
        printf(mem_err);
        exit(1);
   }
   for(i=1;i<=20;i++)    //�।�������� ����� ᫮�� "i" �� 1 �� 20
   {
      SrIS=0;
      for(j=0;j<=i-1;j++)//��室�� �� �ᥬ �����㯯�� ⥪��(�� �ᥣ� i ��.)
      {
         for (k=0;k<=32;k++) Matr[k]=0;
         for(k=0;;k++)//��ॡ�ࠥ� �� �㪢� ���騥 �� �㦭�� �������(���. j+i*k)
         {
            if(j+k*i>=TextLength)break;//�᫮��� �४�饭�� ��ॡ�� �㪢
            //�� ��� int'���� �������⮢ �㪢 ���⠥� "�"(0x80)
            Matr[*(Text+j+i*k)-'�']++; // � ����砥� �᫮ �� 0 �� 32
          }
          Cislo=k;
          IS=0;
          for(k=0;k<=32;k++) IS+=((float)Matr[k]*((float)Matr[k]-(float)1))/((float)Cislo*((float)Cislo-(float)1));//�����뢠�� ������ ᮢ�������
          SrIS+=IS/(float)i;//�����뢠�� �।��� ������ ᮢ�������(�� ��ᥡ�稭� ,��������� �����) �� ������ j �� 㢥��稢����� � � j=i ���ᨬ���� � ���� ����⢨⥫쭮 �।��� �.�.
      }
      if(SrIS>=0.045&&SrIS<=0.065)
      {
        //������塞 ������ � �࠭ ���室�騬� ��. �.�.(SrIS)
        printf("%d. ����� ���祢��� ᫮��=%2d, ������ ᮢ�������=%.3f\n",count+1,i,SrIS);
        *(KeyLength + count++) = i;
      }
      if(debug)
      {
        fprintf(debug,"����� ���祢��� ᫮��=%2d, ������ ᮢ�������=%.3f\n",i,SrIS);
      }
   }
   do
   {
     printf("������ ����� �������� ���室�饩 ����� ���祢��� ᫮��,\n�᭮�뢠��� �� ������ ᮢ������� (⥮���᪮� ���票� ࠢ�� 0.055) ");
     scanf("%d",&j);
     j--;
   }
   while(j>=count);
   printf("����� ���祢��� ᫮�� ࠢ�� %d.\n\n", *(KeyLength+j));
   if(debug) fprintf(debug,"����� ���祢��� ᫮�� ࠢ�� %d.\n\n", *(KeyLength+j));

   i=*(KeyLength+j);
    free(KeyLength);
   return i;
}

/*-------------------���������� ��㯯 �� 䠩��-----------------------*/

int ** tdpGroupsFromFile(char * FileName)//2-�� �㭪��- ࠧ������ �� ��㯯�
{
    int **Groups, i, j;
    char ch;
    FILE *InputFile;
    printf("������� ��㯯�...");
    if((InputFile = fopen(FileName, "rb")) == NULL)
    {
        perror(FileName);
        exit(1);
    }

    /*�뤥����� ����� ��� ������ Groups[KL][AL].*/
    if ((Groups = (int**)calloc(KeyLength, sizeof(int*))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for (i = 0; i < KeyLength; i++)
    {
        if ((Groups[i]=(int*)calloc(ALPHABET_LENGTH, sizeof(int))) == NULL)
        {
            printf(mem_err);
            exit(1);
        }
    }

    i = 0;
    // ������ १���⮢ G(ij)
    while((ch = getc(InputFile)) != EOF)
    {
        if(i == KeyLength)
            i = 0;

        if((ch >= '�') && (ch <= '�'))
        {
            Groups[i++][ch -'�']++;
        }
    }

    fclose(InputFile);

    printf(" ��⮢�.\n\n");

    if(debug)
    {
        fprintf(debug,"������� ��㯯�:\n");
        tdpShowGroups(Groups, KeyLength, ALPHABET_LENGTH);
    }
    return(Groups);
}


/*---------------------Show Groups Matrix------------------------------------*/

void tdpShowGroups(int **M, int max_rows, int max_cols)
{
    int row, col;

    fprintf(debug,"\n   ");
    for (col = 0; col < max_cols; col++)
    {
        fprintf(debug,"%3c", col + '�');
    }

    for (row = 0; row < max_rows; row++)
    {
        fprintf(debug,"\n\n%d  ",row);
        for (col = 0; col < max_cols; col++)
        {
            fprintf(debug,"%3d", M[row][col]);
        }
    }
    fprintf(debug,"\n\n");
}

/*--------------���᫥��� PkXiYj � ���������� ��室��� ������---------------*/

float ***tolyaPkXiYj(int **Groups)
{
   float ***PkXiYj;

   long double SumPkYjXm;
   int k, i, j, m, x, y; //Nk-�᫮ �㪢 � �-�� ��㯯�

   printf("����塞 ������ PkXiYj...");

   //�⢮��� ������ ��� ������ ࠧ��஬ KxN1xN2, ��� K - �᫮ ��㯯;
        //N1 � N2 - ����� ��䠢��,
   //���न��� N1 ᮮ⢥����� �㪢�� ॠ�쭮�� ��䠢��, �
   //���न��� N2 ᮮ⢥����� �㪢�� ��஢������ ��䠢��.
   if((PkXiYj=(float ***)malloc(sizeof(float ***)*KeyLength))==NULL)
   {
      printf(mem_err);
      exit(1);
   }
   for (x=0;x<KeyLength;x++)
   {
        if((PkXiYj[x]=(float **)malloc(sizeof(float **)*ALPHABET_LENGTH))==NULL)
        {
           printf(mem_err);
           exit(1);
        }
        for (y=0;y<ALPHABET_LENGTH;y++)
        if((PkXiYj[x][y]=(float *)malloc(sizeof(float *)*ALPHABET_LENGTH))==NULL)
        {
           printf(mem_err);
           exit(1);
        }
   }

   //������塞 ��室��� ������
   for (k=0; k<KeyLength;k++)
   {
      //�஡����� �� �㪢� ॠ�쭮�� ��䠢��
      for (i=0; i<ALPHABET_LENGTH; i++)
      {
        //�஡����� �� �㪢� k-�� ��㯯� ��஢������ ��䠢��
         for (j=0; j<ALPHABET_LENGTH; j++)
         {
            //����塞 �㬬� PkYjXm ��� 1<=m<=ALPHABET_LENGTH
            SumPkYjXm=0;
            for (m=0;m<ALPHABET_LENGTH; m++)
                SumPkYjXm+=tolyaPkYjXi(Groups, k, j, m);
            //����塞 � ����ᨬ � ������ ���祭�� PkXiYj ��� ⥪��� k, i, � j.
            PkXiYj[k][i][j]=(float)(tolyaPkYjXi(Groups, k, j, i)/SumPkYjXm);
         }
      }
   }
   printf(" ��⮢�.\n\n");

   if(debug)
   {
        fprintf(debug, "����塞 ������ PkXiYj:\n");
        ShowPkXiYjMatrix(PkXiYj,KeyLength,32,32);
   }

   return PkXiYj;
}

/*------------------���᫥��� Pk(Yj|Xi) �ᯮ����⥫쭠� �㭪��--------------*/

//Nk - ��饥 �᫮ �㪢 � k-�� ��㯯�
//Fkj - �᫮ ������ j-�� �㪢� � k-�� ��㯯�
//Pi - �।��� ���� ������ i-�� �㪢� � ���᪮� ��䠢��(������ �� ���ᨢ�
//float rus[ALPHABET_LENGTH];)
long double tolyaPkYjXi(int **Groups, int k, int j, int i)
{
   int Nk=0,n;
   long double PkYjXi;

   //������뢠�� Nk ��� (k-1)-�� ��㯯�
   for (n=0; n<ALPHABET_LENGTH; n++)
      Nk+=Groups[k][n];

        PkYjXi=tdpCnk(Groups[k][j], Nk)*powl(rus[i],Groups[k][j])*powl(1.0-rus[i],(Nk-Groups[k][j]));
   return PkYjXi;
}

/*--------------���᫥��� � �� N �� � (�ᯮ����⥫쭠� �㭪��)-------------*/

long double tdpCnk(unsigned int k, unsigned int n)
{
    unsigned int lb, ub, x;
    long double rez = 1.0;

    if((n - k) < k)
    {
        lb = n - k;
        ub = k + 1;
    }
    else
    {
        ub = n - k + 1;
        lb = k;
    }

    for(x = 1; x <= lb; x++)
        rez /= x;

    for(x = ub; x <= n; x++)
        rez *= x;

    return rez;
}

/*--------����� ����⭮�⥩ ᤢ���� r ����� ��㯯��� k � l (Pklr)---------*/

float*** vovaPklr (float ***PkXiYj)
{
    int k, l, r, x, y;
    float ***Pklr;

    printf("����塞 ������ Pklr...");

    //�뤥����� ����� ��� Pklr.
    if ((Pklr = (float***)calloc(KeyLength, sizeof(float**))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for (x = 0; x < KeyLength; x++)
    {
        if ((Pklr[x]=(float**)calloc(KeyLength, sizeof(float*))) == NULL)
        {
            printf(mem_err);
            exit(1);
        }

        for (y = 0; y < KeyLength; y++)
        {
            if ((Pklr[x][y]=(float*)calloc(ALPHABET_LENGTH, sizeof(float))) == NULL)
            {
                printf(mem_err);
                exit(1);
            }
        }
    }


    for (k=0; k<KeyLength; k++)
    {
        for (l=0; l<KeyLength; l++)
        {
            for (r=0; r<ALPHABET_LENGTH; r++)
            {
                Pklr[k][l][r] = vovaMiddleValue (PkXiYj, k, l, r)/vovaSumma (PkXiYj, k, l);
            }
        }
    }

    /*�᢮�������� PkXiYj(PkXiYj)*/
    for(x = 0; x < KeyLength; x++)
    {
        for(y = 0; y < ALPHABET_LENGTH; y++)
        {
            free(PkXiYj[x][y]);
        }
        free(PkXiYj[x]);
    }
    free(PkXiYj);

    printf(" ��⮢�.\n\n");

    if(debug)
    {
        fprintf(debug,"����塞 ������ Pklr:\n");
        ShowPkXiYjMatrix(Pklr,KeyLength,KeyLength,32);
    }
    return Pklr;
}

/*------��⠥� �஬������ ����稭� Pklr (�ᯮ����⥫쭠� �㭪��)--------*/

float vovaMiddleValue (float*** PkXiYj, int k, int l, int r)
{
    float sumtemp=0, multitemp=1;
    int i, j;

    for (j=0; j<ALPHABET_LENGTH; j++)
    {
        for (i=0; i<ALPHABET_LENGTH; i++)
        {
            sumtemp+=PkXiYj[k][i][j]*PkXiYj[l][(i+r)%ALPHABET_LENGTH][j];
        }
        multitemp*=sumtemp;
    }

    return multitemp;
}

/*��⠥� �������⥫� ��ன ����: �㬬� �� r Pklr(�ᯮ����⥫쭠� �㭪��)*/

float vovaSumma (float ***PkXiYj, int k, int l)
{
    float sumtemp=0;
    int i;

    for (i=0; i<ALPHABET_LENGTH; i++)
    {
        sumtemp+=vovaMiddleValue(PkXiYj, k, l, i);
    }
    return sumtemp;
}



/*---------��宦����� �������樨 ᤢ���� � ���ᨬ��쭮� ����⭮����---------*/

int * tdpSdvigTable(float *** pklr)
{
    int * SdvArr, * SdvArrCopy;

    printf("������� ������ SdvigTable.\n(�������� ��������, �� ����� ������ ����� �६���)...\n");

    if ((SdvArr = (int*)calloc(KeyLength, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    if ((SdvArrCopy = (int*)calloc(KeyLength, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    tdpSdvigs(pklr, SdvArr, SdvArrCopy, 1);

    free(SdvArr);

    printf(" ��⮢�.\n\n");

    return(SdvArrCopy);
}

/*------------�ᯮ����⥫쭠� �㭪�� ��� tdpSdvigTable----------------------*/

void tdpSdvigs(float *** pklr, int * SdvArr, int * SdvArrCopy, int l)
{
    static long double max_p = 1e4000;
    static long double p = 1;
    long double temp;
    int x, k, r;

    if(p > max_p)
    {
        return;
    }

    if(l == KeyLength)
    {
        max_p = p;

        //if(debug) tdpShow1DMatrix(SdvArr, KeyLength);

        memcpy(SdvArrCopy, SdvArr, KeyLength * sizeof(int));

        return;
    }

    for(x = 0; x < ALPHABET_LENGTH; x++)
    {
        SdvArr[l] = x;
        temp = 1;

        for(k = 0; k < l; k++)
        {
            r = (SdvArr[l] - SdvArr[k] + ALPHABET_LENGTH) % ALPHABET_LENGTH;

            temp *= pklr[k][l][r];
        }

        if(temp == 0)
        {
            continue;
        }

        p /= temp;

        tdpSdvigs(pklr, SdvArr, SdvArrCopy, l + 1);

        p *= temp;
    }
}


/*-----------------------������ ���祢��� ᫮��------------------------------*/

char * tdpKeyWord(int *Sdvigs)
{
    int FirstLetter, group;
    char *KeyWord;

    if ((KeyWord = (char*)calloc((KeyLength + 1), sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    clrscr();
    for(FirstLetter = 0; FirstLetter < ALPHABET_LENGTH; FirstLetter++)
    {
        if(FirstLetter % 2)
        {
            printf("\t%d  ",(FirstLetter));
        }
        else
        {
            printf("\n%d  ",(FirstLetter));
        }

        for(group = 0; group < KeyLength; group++)
        {
            printf("%c", ('�' + (FirstLetter - Sdvigs[group] + ALPHABET_LENGTH)%ALPHABET_LENGTH));
        }

    }

    do
    {
        printf("\n ������ ����� ��.\n");
        scanf("%d",&FirstLetter);
    }while((FirstLetter < 0) || (FirstLetter >= ALPHABET_LENGTH));

    for(group = 0; group < KeyLength; group++)
    {
        KeyWord[group] = '�' + (FirstLetter - Sdvigs[group] + ALPHABET_LENGTH)%ALPHABET_LENGTH;
    }
    KeyWord[KeyLength] = '\0';

    return KeyWord;
}


//--------------------------��筥��� ᤢ����--------------------------------//

void tdpCorrectSdvigTable(int * Sdvigs, char * KeyWord)
{
    int number = -1, delta;
    char ch;

    clrscr();

    do
    {
        printf("���祢�� ᫮��: %s\n", KeyWord);

        printf("\n\n����� �� ���浪� �㪢� ᫥��� ��ࠢ���?\n�᫨ �� �ࠢ��쭮, ������ 0\n");
        scanf("\n%d", &number);
        if((number < 1) || (number > KeyLength))
        {
            break;
            //return;
        }
        else
        {
            number--;
            printf("\n\n�� ����� �㪢� �� ᫥��� ��������?\n");
            scanf("\n%c", &ch);

            delta = KeyWord[number] - ch;
            KeyWord[number] = ch;
            Sdvigs[number] = (Sdvigs[number] + delta + ALPHABET_LENGTH) % ALPHABET_LENGTH;
        }
    }while(1);

    if(debug)
    {
        fprintf(debug,"������ ᤢ���� ����� �㪢��� ���祢��� ᫮��:\n");
        tdpShow1DMatrix(Sdvigs, KeyLength);
        fprintf(debug, "���祢�� ᫮��: %s\n\n", KeyWord);
    }

    return;
}



/*-------------���⠢����� ���ᨢ� ����⠭���� (��ਠ�� 3)------------------*/

char * tdpReplaceMatrix3(long double **M)
{
    char *ReplaceMatrix, *used_i;
    int count, i, j, max_i = 0, max_j = 0;
    long double max = -1e1000;

    printf("������� ������ ����⠭����(ReplaseMatrix)...");

    //�뤥����� ����� ��� ���ᨢ�.
    if ((ReplaceMatrix = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    if ((used_i = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for(count = 0; count < ALPHABET_LENGTH; count++)
    {
        for(j = 0; j < ALPHABET_LENGTH; j++)
        {
            for(i = 0; i < ALPHABET_LENGTH; i++)
            {
                if(M[i][j] > max && !used_i[i])
                {
                    max = M[i][j];
                    max_i = i;
                    max_j = j;
                }
            }
        }
        ReplaceMatrix[max_j] = max_i;
        max = -1e1000;
        used_i[max_i] = 1;
    }

    free(used_i);
    for(i=0;i<ALPHABET_LENGTH;i++)
    {
      free(M[i]);
    }
    free(M);
    printf(" ��⮢�.\n\n");

    return (ReplaceMatrix);
}


/*-���⠢����� ������ ����⭮�⥩ ᮢ������� �㪢 �����. � ��室�. ⥪�⮢*/

long double ** tdpPXiYjMatrix(int **Groups, int *r)
{
    int i, j, m;
    long double **M;
    long double sum = 0.0;

    //�뤥����� ����� ��� ������.
    if ((M = (long double **)calloc(ALPHABET_LENGTH, sizeof(long double *))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for (i = 0; i < ALPHABET_LENGTH; i++)
    {
        if ((M[i]=(long double *)calloc(ALPHABET_LENGTH, sizeof(long double))) == NULL)
        {
            printf(mem_err);
            exit(1);
        }
    }


    //����⢥���, ������ p(Xi|Yj) � ����ᥭ�� ��� ����⭮�⥩ � ������.
    for(i = 0; i < ALPHABET_LENGTH; i++)
    {
        for(j = 0; j < ALPHABET_LENGTH; j++)
        {
            sum = 0;
            for(m = 0; m < ALPHABET_LENGTH; m++)
            {
                sum += tdpTmpPXiYj(Groups, r, m, j);
            }

            M[i][j] = (tdpTmpPXiYj(Groups, r, i, j) / sum);
        }
    }

    //�᢮������� Groups
    for(i = 0; i < KeyLength; i++)
    {
        free(Groups[i]);
    }

    free(Groups);
    if(debug)
    {
        fprintf(debug,"����� ������⥩ ᮢ������� �㪢 ��஢������ � ॠ�쭮�� ��䠢�⮢ PXiYj:\n");
        ShowPXiYjMatrix(M,32,32);
    }
    return M;
}

/*----------------�ᯮ����⥫쭠� �㭪�� ��� tdpPXiYjMatrix----------------*/

long double tdpTmpPXiYj(int **Groups,int *r, int i, int j)
{
    int k;
    long double p = 1.0;

    for(k = 0; k < KeyLength; k++)
    {
        p *= tolyaPkYjXi(Groups,k,j,(i + r[k]) % ALPHABET_LENGTH);
    }

    return p;
}


/*-------------------�뢮� ����஢������ ⥪�� � 䠩�---------------------*/

void tdpOutToFile(char * InFileName, char * OutFileName, char *ReplaceMatrix, int *r)
{
    int count = 0;
    char in_char, out_char;
    FILE * OutFile, * InFile;

    printf("�����஢뢠�� 䠩� %s � 䠩� %s...", InFileName, OutFileName);

    if((InFile = fopen(InFileName, "r")) == NULL)
    {
        perror(InFileName);
        exit(1);
    }

    if((OutFile = fopen(OutFileName, "w")) == NULL)
    {
        perror(OutFileName);
        exit(1);
    }

    while((in_char = getc(InFile)) != EOF)
    {
        if((in_char >= '�') && (in_char <= '�'))
        {
            out_char = ((ReplaceMatrix[in_char - '�'] + r[count % KeyLength]) % ALPHABET_LENGTH + '�');
            fputc(out_char, OutFile);
            count++;
        }
    }
    free(ReplaceMatrix);
    free(r);
    printf(" ��⮢�.\n\n");
}

/*------------------�뢮� ����஢������ ⥪�� � ���ᨢ--------------------*/

char * tdpOutToArray(char *String, char *ReplaceMatrix, int *r)
{
    int count;
    char *OutArray;

    if ((OutArray = (char*)calloc(strlen(String)+1/*SIZE_OF_OUT_ARRAY*/, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for(count = 0; (String[count] != '\0')/* && (count < (SIZE_OF_OUT_ARRAY - 1))*/; count++)
    {
//        OutArray[count] = (ReplaceMatrix[(String[count] - '�' - r[count % KeyLength] + ALPHABET_LENGTH) % ALPHABET_LENGTH] + '�');
        OutArray[count] = ((ReplaceMatrix[String[count] - '�'] + r[count % KeyLength]) % ALPHABET_LENGTH + '�');
    }
    OutArray[count] = '\0';

    return (OutArray);
}

/*-----------------------��筥��� ReplaceMatrix-----------------------------*/

char * tdpCorrectReplaceMatrix(char * In, char * Out, char * RM, int * r, char *KeyWord)
{
    char ch;//�����塞� ᨬ���.
    int n;//����� ��ࠡ��뢠���� �㪢� � ���ᨢ�.

    while(1)
    {
        clrscr();

        tdpShowDecryptedText(Out);

        do
        {
            printf("\n�㦭� �� ��ࠢ�����? (�/�)(%f): \n",IsRussianText(Out,strlen(Out)));
            scanf("\n%c",&ch);
        }while((ch != '�') && (ch != '�'));

        if(ch == '�')
            break;

        do
        {
            printf("\n������ ����� �㪢�:\n");
            scanf("%d",&n);
        }while((n < 0) || (n >= SIZE_OF_OUT_ARRAY));

        do
        {
            printf("\n�������� �㪢� %c ��:\n", Out[n]);
            scanf("\n%c",&ch);
        }while((ch < '�') || (ch > '�'));

        RM[In[n] - '�'] = (ch - '�' - r[n % KeyLength] + ALPHABET_LENGTH) % ALPHABET_LENGTH;

        Out = tdpOutToArray(In, RM, r);
    }
    if(debug)
    {
        char *Viz;
        int count;
        fprintf(debug,"\n����⠭���筠� �����:\n");
        tdpShow1DCharMatrix(RM, ALPHABET_LENGTH);
        if ((Viz = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
        {
            printf(mem_err);
            return RM;
        }
        fprintf(debug,"��室�� ������ �������:\n");
        //����砥� ����� ��ப� ������ �������
        for(n=0;n<ALPHABET_LENGTH;n++)
        {
            Viz[(RM[n]+(KeyWord[0]-'�'))%ALPHABET_LENGTH]=n+'�';
        }

        //�뢮��� ���� ������
        for(n=0;n<ALPHABET_LENGTH;n++)
        {
            fwrite(Viz, sizeof(char), ALPHABET_LENGTH, debug);
            fprintf(debug, "\n");
            ScrollLeft(Viz);
        }
        free(Viz);
    }

    free(In);
    free(Out);

    return RM;
}

//������᪨� ᤢ�� ����⮢ ���ᨢ� ��ࠢ�
void ScrollRight(char *array)
{
    int n;
    char ch=array[ALPHABET_LENGTH-1];
    for(n=ALPHABET_LENGTH-1;n!=0;n--)
    {
        array[n]=array[n-1];
    }
    array[0]=ch;
}

//������᪨� ᤢ�� ����⮢ ���ᨢ� �����
void ScrollLeft(char *array)
{
    int n;
    char ch=array[0];
    for(n=0;n!=ALPHABET_LENGTH-1;n++)
    {
        array[n]=array[n+1];
    }
    array[ALPHABET_LENGTH-1]=ch;
}



void tdpShowDecryptedText(char * Out)
{
    int count, tens = 0, row;

    for(row = 0; row < 3; row++)
    {
        for(count = 0; count < 8; count++)
        {
            printf("%-3d       ", tens++);
        }

        for(count = 0; count < 80; count++)
        {
            printf("%d", count % 10);
        }
        for(count = 0; count < 80; count++)
        {
            printf("%c", Out[count + row * 80]);
        }
        printf("\n");
    }
}
/*--------------------��ॢ���� ���祢�� ᫮�� � ᤢ���----------------------*/
int *word2sdvig(unsigned char *buff)
{
   int i;
   int len=KeyLength;//strlen(buff);
   int *sdvigs;
   if ((sdvigs = (int*)calloc(len, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

   sdvigs[0]=0;
   for (i=1;i<len;i++)
   {
      sdvigs[i]=(buff[0]-buff[i]+32)%32;

   }
   return sdvigs;
}

float IsRussianText(char *buff, long len)
{
    long LtrCount[ALPHABET_LENGTH];
    float LtrFreq[ALPHABET_LENGTH];
    int count;
    float result=0.0;
    long LtrCnt=0;
    for(count=0;count<ALPHABET_LENGTH;count++)
    {
        LtrCount[count]=0;
    }
    /*for(count=0; count<(len/0xFFFF);count++)
    {
        LtrCnt+=AddLtr(buff, LtrCount, 0xFFFF);
    }
    LtrCnt+=AddLtr(buff, LtrCount, len%0xFFFF);*/

    //for(count=0; count<len;count++)
    //{
        LtrCnt+=AddLtr(buff, LtrCount, len);
    //}
    for(count=0;count<ALPHABET_LENGTH;count++)
    {
        LtrFreq[count]=(float)LtrCount[count]/(float)LtrCnt;
        result+=(fabs(1.0-LtrFreq[count]/rus[count]))*(fabs(rus[count]-LtrFreq[count]));
    }
    return result;
}



int AddLtr(char *buff, long *LtrCount, int len)
{
    int i;
    int num;
    int count=0;
    for(i=0;i<len;i++)
    {
        num=toupper_rus(buff[i]);
        if(num>0x7F && num<0xA0)
        {
            LtrCount[num-0x80]++;
            count++;
        }
    }
    return count;
}

unsigned int toupper_rus(unsigned char ch)
{
   if(ch>=160&&ch<=175) {ch-=32; return ch;}
   if(ch>=224&&ch<=239) {ch-=80; return ch;}
   return ch;
}



/*===========================================================================*/

/*=========================������� �㭪樨===================================*/



/*-----------------------�।����, �� � -log  -----------------------------*/

long ** tdpPXiYjMatrix2(int **Groups, int *r)
{
    int i, j, m;
    long **M;
    long double sum = 0.0;

    //�뤥����� ����� ��� ������.
    if ((M = (long **)calloc(ALPHABET_LENGTH, sizeof(long *))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for (i = 0; i < ALPHABET_LENGTH; i++)
    {
        if ((M[i]=(long *)calloc(ALPHABET_LENGTH, sizeof(long))) == NULL)
        {
            printf(mem_err);
            exit(1);
        }
    }


    //����⢥���, ������ p(Xi|Yj) � ����ᥭ�� ��� ����⭮�⥩ � ������.
    for(i = 0; i < ALPHABET_LENGTH; i++)
    {
        for(j = 0; j < ALPHABET_LENGTH; j++)
        {
            sum = 0;
            for(m = 0; m < ALPHABET_LENGTH; m++)
            {
                sum += tdpTmpPXiYj(Groups, r, m, j);
            }

            M[i][j] = (long)(- log10l(tdpTmpPXiYj(Groups, r, i, j) / sum));
        }
    }

    //�᢮������� Groups (from tolyaPkXiYj)
    for(i = 0; i < KeyLength; i++)
    {
        free(Groups[i]);
    }

    free(Groups);
//    ShowPXiYjMatrix(M,32,32);
//    fflush(stdout);

    return M;
}


/*-------------���⠢����� ���ᨢ� ����⠭���� (��ਠ�� 1)------------------*/

char * tdpReplaceMatrix(long double **M)
{
    char *ReplaceMatrix, *used_i;
    int i, j, max_i = 0, max_j = 0;

    //�뤥����� ����� ��� ���ᨢ�.
    if ((ReplaceMatrix = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    if ((used_i = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
       printf(mem_err);
        exit(1);
    }


    //���⠢����� ���ᨢ� ����⠭����.
    for(j = 0; j < ALPHABET_LENGTH; j++)
    {
        for(i = 0; i < ALPHABET_LENGTH; i++)
        {
            if((M[i][j] > M[max_i][j]) /*&& !used_i[i]*/)
               max_i = i;
        }
        ReplaceMatrix[j] = max_i;
        used_i[max_i] = 1;
    }

//    tdpReplaces(M, TmpRepM, ReplaceMatrix, 0);

    //�᢮�������� used_i � M(PXiYj).
//    free(used_i);

    for(i = 0; i < ALPHABET_LENGTH; i++)
    {
        free(M[i]);
    }

    free(M);


    return ReplaceMatrix;
}

/*-------------���⠢����� ���ᨢ� ����⠭���� (��ਠ�� 2)------------------*/

char * tdpReplaceMatrix2(long **M)
{
    char *ReplaceMatrix, *TmpRepM;
    int i;

    //�뤥����� ����� ��� ���ᨢ�.
    if ((ReplaceMatrix = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    if ((TmpRepM = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    tdpReplaces(M, TmpRepM, ReplaceMatrix, 0);

    //�᢮�������� M(PXiYj).

    for(i = 0; i < ALPHABET_LENGTH; i++)
    {
        free(M[i]);
    }

    free(M);


    return ReplaceMatrix;
}

/*------------------�ᯮ����⥫쭠� ��� tdpReplaceMatrix2--------------------*/

void tdpReplaces(long ** M, char * RepM, char * RepMCopy, char y)
{
    static long double max_p = 1e1000;
    static long double p = 0;
    static int used[ALPHABET_LENGTH];
    long double temp;
    char x;

    if(p > max_p)
    {
        return;
    }

    if(y == ALPHABET_LENGTH)
    {
        max_p = p;

        tdpShow1DCharMatrix(RepM, ALPHABET_LENGTH);

        memcpy(RepMCopy, RepM, ALPHABET_LENGTH * sizeof(char));

        return;
    }

//    temp = 0;

    for(x = 0; x < ALPHABET_LENGTH; x++)
    {
        if(!used[x])
        {
            temp = M[x][y];

            if(temp <= 0)
            {
                //temp = 0;
                continue;
            }

            used[x] = 1;
            RepM[y] = x;

            p += temp;

            tdpReplaces(M, RepM, RepMCopy, y + 1);

            p -= temp;

            used[x] = 0;
        }
    }
}

/*-------------���⠢����� ���ᨢ� ����⠭���� (��ਠ�� 4)------------------*/

char * tdpReplaceMatrix4(int ** Groups, int * Sdvigs)
{
    float Raspred[ALPHABET_LENGTH], max_freq;
    long N = 0, n;
    int group, letter, count, x, max_x;
    char rus_sorted[ALPHABET_LENGTH] = {'�', '�', '�', '�', '�', '�', '�', '�',
                                        '�', '�', '�', '�', '�', '�', '�', '�',
                                        '�', '�', '�', '�', '�', '�', '�', '�',
                                        '�', '�', '�', '�', '�', '�', '�', '�'};
    char * ReplaceMatrix;

    if ((ReplaceMatrix = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for(group = 0; group < KeyLength; group++)
    {
        for(letter = 0; letter < ALPHABET_LENGTH; letter++)
        {
            N += Groups[group][letter];
        }
    }


    for(letter = 0; letter < ALPHABET_LENGTH; letter++)
    {
        n = 0;

        for(group = 0; group < KeyLength; group++)
        {
            n += Groups[group][(letter - Sdvigs[group] + ALPHABET_LENGTH) % ALPHABET_LENGTH];
        }

        Raspred[letter] = (float)n / N;
    }

    for(count = 0; count < ALPHABET_LENGTH; count++)
    {
        max_freq = 0;

        for(x = 0; x < ALPHABET_LENGTH; x++)
        {
            if( Raspred[x] > max_freq)
            {
                max_freq = Raspred[x];
                max_x = x;
            }
        }

        ReplaceMatrix[max_x] = rus_sorted[count] - '�';
        Raspred[max_x] = 0;
    }

    return ReplaceMatrix;
}

/*---------------------Show 1-d Matrix---------------------------------------*/

void tdpShow1DMatrix(int *M, int max_cols)
{
    int col;
    for (col = 0; col < max_cols; col++)
    {
        fprintf(debug," %d", M[col]);
    }
    fprintf(debug,"\n\n");
}


/*---------------------Show 1-d Char Matrix----------------------------------*/

void tdpShow1DCharMatrix(char *M, int max_cols)
{
    int col;
    for (col = 0; col < max_cols; col++)
    {
        fprintf(debug," %d", M[col]);
    }
    fprintf(debug,"\n\n");
}

/*---------------------Show 2-d Matrix---------------------------------------*/

void tdpShow2DMatrix(int **M, int max_rows, int max_cols)
{
    int row, col;

    for (row = 0; row < max_rows; row++)
    {
        for (col = 0; col < max_cols; col++)
        {
            printf(" %2d", M[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

/*---------------------Show 3-d Matrix---------------------------------------*/

void tdpShow3DMatrix(int ***M, int max_z, int max_y,int max_x)
{
    int x, y, z;

    for (z = 0; z < max_z; z++)
    {
        for (y = 0; y < max_y; y++)
        {
            for (x = 0; x < max_x; x++)
            {
                fprintf(debug," %2d",M[z][y][x]);
            }
            fprintf(debug,"\n");
        }
        fprintf(debug,"\n\n");
    }
}

/*---------------------Show Converted Pklr Matrix----------------------------*/

void tdpShowConvPklr(int ***M, int max_k, int max_l,int max_p)
{
    int k, l, p;

    for (p = 0; p < max_p; p++)
    {
        for (k = 0; k < max_k; k++)
        {
            for (l = 0; l < max_l; l++)
            {
                fprintf(debug, " %2d",M[k][l][p]);
            }
            fprintf(debug,"\n");
        }
        fprintf(debug,"\n\n");
    }
}

/*-------------------��宦����� ᤢ���� �� 1 ��� �����---------------------*/

int * tdpSdvigTable2(int ** Groups)
{
    int x, a, r;
    int * f_i, * f_j, * SdvArr;

    if ((f_i = (int*)calloc(ALPHABET_LENGTH, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    if ((f_j = (int*)calloc(ALPHABET_LENGTH, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    if ((SdvArr = (int*)calloc(KeyLength, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }


    f_j = Groups[0];
    SdvArr[0] = 0;

    for(x = 1; x < KeyLength; x++)
    {
        f_i = Groups[x];
        r = tdpCount_r(f_i, f_j);
        for(a = 0; a < ALPHABET_LENGTH; a++)
        {
            f_j[a] = f_j[a] + f_i[(a - r) % ALPHABET_LENGTH];
        }
        SdvArr[x] = r;
    }

    return(SdvArr);
}

//�ᯮ����⥫쭠� �㭪�� (��� �।��饩).
int tdpCount_r(int * f_i, int * f_j)
{
    int r, x, rez = 0;
    long R, maxR = 0;

    for(r = 0; r < ALPHABET_LENGTH; r++)
    {
        R = 0;

        for(x = 0; x < ALPHABET_LENGTH; x++)
        {
            R += f_j[x] * f_i[(x - r) % ALPHABET_LENGTH];
        }

        if(R > maxR)
        {
            rez = r;
            maxR = R;
        }
    }

    return (rez);
}

/*---------------�८�ࠧ������ ���ᨢ� ᤢ���� � ������--------------------*/

int ** tdpSdvigMatrix(int * SdvArr)
{
    int ** SdvigTable;
    int k, l;

   /*�뤥����� ����� ��� ������.*/
    if ((SdvigTable = (int**)calloc(KeyLength, sizeof(int*))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }

    for (l = 0; l < KeyLength; l++)
    {
        if ((SdvigTable[l]=(int*)calloc(KeyLength, sizeof(int))) == NULL)
        {
            printf(mem_err);
            exit(1);
        }
    }

    // ���⠢����� ⠡���� ᤢ���� �� ᤢ���� �⭮�⥫쭮 1-� �㪢�.
    for(k = 0; k < KeyLength; k++)
    {
        for(l = 0; l < KeyLength; l++)
        {
            SdvigTable[k][l] = (SdvArr[l] - SdvArr[k] + ALPHABET_LENGTH) % ALPHABET_LENGTH;
        }
    }

    return (SdvigTable);
}



//=============================�����=========================================//

void tdpSdvigs2(float *** pklr, int * SdvArr, int * SdvArrCopy, int l)
{
    static long double max_p = 1e4000;
    static long double p = 1;
    long double temp;
    int x, k, r;

    if(p > max_p)
    {
        return;
    }

    if(l == KeyLength)
    {
//        int * ptr;

        max_p = p;

        tdpShow1DMatrix(SdvArr, KeyLength);

        memcpy(SdvArrCopy, SdvArr, KeyLength * sizeof(int));

        return;
    }

    for(x = 0; x < ALPHABET_LENGTH; x++)
    {
        SdvArr[l] = x;
        temp = 0;

        for(k = 0; k < l; k++)
        {
            r = (SdvArr[l] - SdvArr[k] + ALPHABET_LENGTH) % ALPHABET_LENGTH;

            temp += 1/pklr[k][l][r];
        }

/*        if(temp == 0)
        {
            continue;
        }
*/
        p += temp;

        tdpSdvigs(pklr, SdvArr, SdvArrCopy, l + 1);

        p -= temp;
    }
}

void tdpSdvigs3(float *** pklr, int * SdvArr, int * SdvArrCopy, int n)
{
    static long double maxsum = 0;
    int x;

    if(!n)
    {
        long double t;

        tdpShow1DMatrix(SdvArr, KeyLength);

        t = tdpCountSum(pklr, SdvArr);
        if(t > maxsum)
        {
            maxsum = t;
            memcpy(SdvArrCopy, SdvArr, KeyLength * sizeof(int));
        }
        return;
    }

    for(x = 0; x < ALPHABET_LENGTH; x++)
    {
        SdvArr[n] = x;
        tdpSdvigs(pklr, SdvArr, SdvArrCopy, n - 1);
    }
}


long double tdpCountSum(float *** pklr, int * SdvArr)
{
    int k, l, r;
    long double t = 1;

    for(k = 0; k < KeyLength; k++)
    {
        for(l = (k+1); l < KeyLength; l++)
        {
            r = (SdvArr[l] - SdvArr[k] + ALPHABET_LENGTH) % ALPHABET_LENGTH;
            t *= pklr[k][l][r];
        }
    }
    return(t);
}


void tdpCheck(int ***r)
{
    int k=0, l=1, m=2, p1, p2, p3;
    int prec = 9, found=0;

/*    for(k = 0; k < KeyLength; k++)
    {
        for(m = 0; m < KeyLength; m++)
        {
            for(l = 0; l < KeyLength; l++)
            {
                if((k==m)||(k==l)||(l==m))
                {
                    continue;
                }
*/
                for(p1 = 0; p1 < prec; p1++)
                {
                    for(p2 = 0; p2 < prec; p2++)
                    {
                        for(p3 = 0; (p3 < prec)&&(!found); p3++)
                        {
                            if((r[k][l][p1] + r[l][m][p2]) % ALPHABET_LENGTH == r[k][m][p3])
                            {
                                found = 1;
                                printf("found");
                            }
                        }
                        if(!found)
                        {
//                            r[k][l][p1] = -1;
//                            r[l][m][p2] = -1;
                        }
                        else
                        {
                            found = 0;
                        }
                    }
                }
/*
            }
        }
    }
*/
}

/*==========================�㭪樨 �����====================================*/

//-------------------------
int igorFind(int k,int r,int *line)
{
   int i;
   for(i=0;i<r;i++)
   {
      if(line[i]==k) return(1);
   }
   return(0);
}
//-------------------------
/*==========================������� �㭪樨==================================*/
/*===========================================================================*/
// ���� ��ன �㭪樨
// ���⠢����� �������� ������, � ���ன �� ���浪� ᫥������� ����
// ����稭� ᤢ���� ��� 䨪�஢����� ���⪨ k*l

///////////////////////////////////////////////////////////////////////////////
// ��� ���ᨬ����� ����⭮��� � �����뢠�� �㤠 0, ��᫥ 祣� �����頥�
// ������ �����

int vovaFindMax (float *matrix)
{
 int temp=0, i;
 float ftemp=0.0;
 for (i=0; i<ALPHABET_LENGTH; i++)
 {
  if (ftemp<matrix[i])
  {
   ftemp = matrix[i];
   temp = i;
  }
 }
 matrix [temp] = 0;
 return temp;
}

///////////////////////////////////////////////////////////////////////////////
// �����।�⢥��� �������� �������� ������ ⨯� int*

int* vovaConvert (float ***matrix, int k, int l)
{
    int *convertedliniarmatrix, i;
    // int convertedliniarmatrix[ALPHABET_LENGTH];

    float *KL_Line;
    //�뤥����� ����� ��� convertedliniarmatrix
    if ((convertedliniarmatrix = (int*)calloc(ALPHABET_LENGTH, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }
    //�뤥����� ����� ��� KL_Line
    if ((KL_Line = (float*)calloc(ALPHABET_LENGTH, sizeof(float))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }
    //Copy matrix[k][l] to KL_Line
    for (i=0; i<ALPHABET_LENGTH; i++)
    {
      KL_Line[i]=matrix[k][l][i];
    }

 for (i=0; i<ALPHABET_LENGTH; i++)
 {
  convertedliniarmatrix[i] = vovaFindMax (KL_Line);
 }
 free(KL_Line);
 return convertedliniarmatrix;
}

/*==========================������ �㭪樨===================================*/
// Show PkXiYj matrix
void ShowPkXiYjMatrix(float ***M, int max_z, int max_y,int max_x)
{
    int x, y, z;

    for (z = 0; z < max_z; z++)
    {
        for (y = 0; y < max_y; y++)
        {
            for (x = 0; x < max_x; x++)
            {
                fprintf(debug," %.8f",M[z][y][x]);
            }
            fprintf(debug,"\n");
        }
        fprintf(debug,"\n\n");
    }
}
// Show PXiYj matrix
void ShowPXiYjMatrix(long double **M, int max_y,int max_x)
{
    int x, y;

    for (y = 0; y < max_y; y++)
    {
      for (x = 0; x < max_x; x++)
      {
         fprintf(debug," %.8Lf",M[y][x]);
      }
      fprintf(debug,"\n");
    }
}
//���������� ������ ����⭮�⥩ Pklr � ������ ᤢ����
int ***ConvertPklr(float ***Pklr)
{
    int ***ConvertedPklr;
    int i,k,l;
    float *KL_Line;
    if((ConvertedPklr=(int ***)malloc(sizeof(int ***)*KeyLength))==NULL)
    {
        printf(mem_err);
      exit(1);
    }
    for (k=0;k<KeyLength;k++)
    {
        if((ConvertedPklr[k]=(int **)malloc(sizeof(int **)*KeyLength))==NULL)
           {
                printf(mem_err);
                exit(1);
           }
   for (i=0;i<KeyLength;i++)
        if((ConvertedPklr[k][i]=(int *)malloc(sizeof(int *)*ALPHABET_LENGTH))==NULL)
           {
                printf(mem_err);
                exit(1);
           }
    }
    //�뤥����� ����� ��� KL_Line
    if ((KL_Line = (float*)calloc(ALPHABET_LENGTH, sizeof(float))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }
    for (k=0;k<KeyLength;k++)
        for (l=0;l<KeyLength;l++)
        {
            //Copy Pklr[k][l] to KL_Line
            for (i=0; i<ALPHABET_LENGTH; i++)
            {
                KL_Line[i]=Pklr[k][l][i];
            }
            for (i=0; i<ALPHABET_LENGTH; i++)
            {
                ConvertedPklr[k][l][i] = vovaFindMax (KL_Line);
            }
        }
    free(KL_Line);
    for(i=0;i<KeyLength;i++)
    {
        for(k=0;k<KeyLength;k++)
      free(Pklr[i][k]);
      free(Pklr[i]);
    }
    free(Pklr);
    return ConvertedPklr;
}

void put(char *data)
{
    queue *new;
    //printf("PUT: %s\n",data);
    new=(queue *)calloc(sizeof(queue),1);
    if(bottom!=NULL) bottom->p=new;
    if (top==NULL) top=new;
    new->data=data;
    new->p=NULL;
    bottom=new;
}
char *get(void)
{
    char *data=NULL;
    queue *old=top;
    if(top!=NULL)
    {
        data=top->data;
        top=top->p;
        free(old);
    }
    //printf("Get: %s\n", data);
    return data;
}


char **InitSearch(void)
{
    char buff[3];
    char **dict;
    int handle, i;
    
    printf("\n����㦠�� ᫮����...");
    if((handle=_rtl_open((const char*)itoa(KeyLength,buff,10),O_RDONLY|SH_DENYWR))==-1)
    {
        printf(" ������� �� ������!\n");
        return NULL;
    }
    items=filelength(handle)/(KeyLength+1);
    if((dict=(char **)farcalloc(items,sizeof(char *)))==NULL)
    {
        printf(" ���������� �⢥�� ������ ��� ᫮����!\n");
        return NULL;
    }

    for(i=0;i<items;i++)
    {
        if((dict[i]=(char *)calloc(KeyLength+1,sizeof(char)))==NULL)
        {
            printf(" ���������� �⢥�� ������ ��� ᫮����!\n");
            for(;i>=0;i--)
            {
                free(dict[i]);
            }
            return NULL;
        }
        _rtl_read(handle,dict[i],KeyLength+1);
    }
    printf(" ��⮢�.\n");
    return dict;
}

void DeInitSearch(char **dict)
{
    int i;
    for(i=0;i<items;i++)
    {
        free(dict[i]);
    }
    farfree(dict);
}

//�㭪�� ��ࠢ����� ��ப
int strncomp(const void *str1, const void *str2)
{
    int i=0;
    //printf("Comparing: %s<-->%s(%Fp)\n",str1,str2);
    while(!(*((unsigned char *)str1+i)-*((unsigned char *)str2+i)) && i<(KeyLength-1)) i++;
    return (*((unsigned char *)str1+i)-*((unsigned char *)str2+i));
}

//������ ����, ᫮�� � ᫮���

int BinarySearch(char **dict, char *data)
{
    int i=1, j=items,k, result;
    do
    {
        k=(i+j)/2;
        result=strncomp(data, dict[k]);
        //printf("result: %d\n", result);
        if((result)>0) i=k+1;
        else j=k-1;
    }
    while(result && i < j);
    if(result) return 0;
    else
    {
        //put(dict[k]);
        return 1;
    }
}

//������塞 ⠡���� �� �᭮�� ��ࢮ� ��ப�
void calculate(int **table)
{
   int i,j,k;
   for(i=2;i<KeyLength;i++)
   {
      for(j=0;j<i-1;j++)
      {
         for(k=j;k<i;k++)
         {
            //printf("i=%d j=%d k=%d table=%d\n", i,j,k,table[k][k+1]);
            table[j][i]+=table[k][k+1];
         }
         table[j][i]%=ALPHABET_LENGTH;
      }
   }
}

//����塞 ⠡����
void zero(int **table)
{
   int i,j;
   for(i=2;i<KeyLength;i++)
   {
      for(j=0;j<i-1;j++)
      {
         table[j][i]=0;
      }
   }

}

//�஢��塞 ����⠭��� ⠡���� �� ����� ConvertedPklr
int check(int **table, int ***ConvertedPklr, int prec)
{
   int i, j, k, errors=0, val;
   for(i=2;i<KeyLength;i++)
   {
      for(j=0;j<i-1;j++)
      {
         val=table[j][i];
         for(k=0;k<prec;k++)
         {
            if(ConvertedPklr[j][i][k]==val) break;
         }
         if(k==prec) errors++;
      }
   }
   //printf("Errors: %d\n", errors);
   return errors;
}

//��ॡ�� ���祢�� ᫮� �� �᭮�� ConvertedPklr
//�������� ������ ᫮�� �஢������� ���묨
/* ���ᠫ��� �����⬠ ��ॡ��, ��� 0.5� �� ࠧ�������.
���ᨡ� 2:5030/1158.14!
 ��� � �� 0 �� �-1 (� - �᫮ �⮫�殢) � 横��:
  ��ࠡ��뢠��� �� �᫠, ᮤ�ঠ騥 ���� �� 0 �� �. �� ���� �᫠, ����騥
  �� "�" � "?". (�� �⮬, �᫮ ??? ���� �᪫����, ���, � ���� _���_ ��
  ᠬ�� ����, 㦥 �뫨.) ��� �� ᤥ����, �᫨ �=0 ����⭮, (�� ���� 0) �
  �᫨ ���? ����� ⠪.
  ��� � �� 1 �� 10^�-1 (� ����筮� ��⥬�, � ���� �� 2^�-1) � 横��:
   �������� � �-���筮� ����� �᫠ � ����� 0 �� ?.
   �������� ������ 1 �� �.
   � �ਭ樯�, ���� ���� ��������� ���ᨢ � �ᥫ ��ࠬ� � � ���������
   ��᪮�쪮 㪠��⥫�� �� � �祩��, ��� ������ ���� "?".
   ��᫮ "?" (�᫮ ����ᮢ) ��⠥��� �� 室� ���� � ������������ ��� �. �᫨
   ��� �� ����, � ����� ⠪.
   ��� ������� � �� 0 �� (�-1)^�-1, � (�-1)-�筮� ��⥬� ��᫥��� � 横��:
    ����⠢�塞 � "����" ���� �᫠ �, ���� � �⮩ ��⥬� ��᫥��� � ���
    ࠧ �-���筮�. ����, ������ �ࠧ� �ᯮ�짮���� ���ᨢ 㪠��⥫�� ���
    ����㯠 � ��ࠬ �᫠ �.
    ����� - ��ࠡ��뢠��� ����祭��� �� ��᪥ �᫮, �� � ���� ��।��� xyz.
*/

void enumerate(int ***ConvertedPklr)
{
   int i, j, **table, prec, errors;
   char *a, *index, *sub_index;
   int k, l, p, q;
   long count=0;
   char *data;

   char **dict;

   printf("����쪮 �������� ������� ᤢ���� �㤥� �ᯮ�짮����\n�� �஢�ન ���४⭮�� ⠡���� ᤢ����(���祭�� �� 1 �� 32): ");
   scanf("%d", &prec);
   printf("\n������ ���ᨬ��쭮 �����⨬�� �᫮ ��ᮮ⢥�⢨� ����� ⠡��殩 ᤢ����\n� ����楩 ConvertedPklr - �᫮ ��������� �訡�� � ⠡��� ᤢ����,\n��������饥 �⭥�� ��� ⠡���� � �����⨬�\n(����� ��� ������� �᫠ �� 0 �� 3, �� �� �� ���筮): ");
   scanf("%d", &errors);

   if( (table=(int **)calloc(KeyLength,sizeof(int *)))==NULL )
   {
      printf(mem_err);
      exit(1);
   }
   for(i=0;i<KeyLength;i++)
   {
      if((table[i]=(int *)calloc(KeyLength,sizeof(int)))==NULL)
      {
         printf(mem_err);
         exit(1);
      }
   }

   if((data=(char *)calloc(KeyLength+1,sizeof(char)))==NULL)
   {
      printf(mem_err);
      exit(1);
   }

   //����㦠�� ᫮����
   if((dict=InitSearch())==NULL)
   {
      //printf("Not enouth memory to load dictionary!\n");
      exit(1);
   }
   //ShowDict(dict);

    //���� ������� ����⠭����
    if((a=(char *)malloc((KeyLength-1)*sizeof(char)))==NULL)
    {
        printf(mem_err);
        exit(1);
    }
    if((index=(char *)calloc(KeyLength, sizeof(char)))==NULL)
    {
        printf(mem_err);
        exit(1);
    }


    for(k=0;k<prec;k++)
    {
        memset((void *)a, 0, KeyLength-1);
        for(;add(a,KeyLength-1);)
        {
            //������塞 ���祭��� k � ��⠥� �᫮ ����ᮢ
            p=0;
            for(l=KeyLength-1;l>=0;l--)
            {
                if(!a[l]) p++;
                else index[l]=k;
            }

            if(!p)
            {
                //��砫� ᥪ樨
                count++;
                //������塞 ⠡���� ᤢ���� ��砫�묨 ���祭�ﬨ
                for(i=0;i<KeyLength-1;i++)
                {
                    table[i][i+1]=ConvertedPklr[i][i+1][index[i]];
                }
                //����塞 ⠡���� ᤢ���� 楫����
                calculate(table);
                //�஢��塞
                if(check(table, ConvertedPklr, prec)<=errors)
                {
                    //����ਬ �� ᫮�� �� ᤢ���� � �஢��塞 �� ᫮����
                    for(i = 0; i < ALPHABET_LENGTH; i++)
                    {
                        for(j = 0; j < KeyLength; j++)
                        {
                            data[j]=0x80 + (i - table[0][j] + ALPHABET_LENGTH)%ALPHABET_LENGTH;
                            data[KeyLength]=0;
                        }
                        if(BinarySearch(dict, data))
                        {
                            printf("������� ���祢�� ᫮��: %s\n",data);
                            if(debug)
                            {
                                fprintf(debug, "-----------------\n������� ���祢�� ᫮��: %s\n", data);
                                fshow(table);
                            }
                        }
                    }
                }
                zero(table);
                //����� ᥪ樨
                break;
            }
            if((sub_index=(char *)calloc(p, sizeof(char)))==NULL)
            {
                printf(mem_err);
                exit(1);
            }
            //������塞 ������ ���祭�ﬨ
            while(sub_index[0]!=k)
            {
                p=0;
                for(l=KeyLength-1;l>=0;l--)
                {
                    if(!a[l]) index[l]=sub_index[p++];
                }
                p--;
                sub_index[p]++;

                //��砫� ᥪ樨
                count++;
                //������塞 ⠡���� ᤢ���� ��砫�묨 ���祭�ﬨ
                for(i=0;i<KeyLength-1;i++)
                {
                    table[i][i+1]=ConvertedPklr[i][i+1][index[i]];
                }
                //����塞 ⠡���� ᤢ���� 楫����
                calculate(table);
                //�஢��塞
                if(check(table, ConvertedPklr, prec)<=errors)
                {
                    //����ਬ �� ᫮�� �� ᤢ���� � �஢��塞 �� ᫮����
                    for(i = 0; i < ALPHABET_LENGTH; i++)
                    {
                        for(j = 0; j < KeyLength; j++)
                        {
                            data[j]=0x80 + (i - table[0][j] + ALPHABET_LENGTH)%ALPHABET_LENGTH;
                            data[KeyLength]=0;
                        }
                        if(BinarySearch(dict, data))
                        {
                            printf("������� ���祢�� ᫮��: %s\n",data);
                            if(debug)
                            {
                                fprintf(debug, "-----------------\n������� ���祢�� ᫮��: %s\n", data);
                                fshow(table);
                            }
                        }
                    }
                }
                zero(table);
                //����� ᥪ樨

                for(;p>0;p--)
                    if(sub_index[p]==(k))
                    {
                        sub_index[p-1]++;
                        sub_index[p]=0;
                    }
            }
            free(sub_index);
        }
    }

   for(i=0;i<KeyLength;i++)
   {
      free(table[i]);
   }
   free(a);
   free(index);
   free(table);
   free(data);
   for(i=0;i<KeyLength;i++)
   {
    for(j=0;j<KeyLength;j++)
      free(ConvertedPklr[i][j]);
      free(ConvertedPklr[i]);
   }
   free(ConvertedPklr);

//��ᬠ�ਢ��� ᫮�� �� 㤠����� ᫮����!!!(�� �ᯮ�짮����� ��।� put, get)

   //����塞 ᫮����
   DeInitSearch(dict);
   printf("��ࠡ�⠭� ��ਠ�⮢: %d\n", count);
}

void show(int **table)
{
   int i,j;
   for(i=0;i<KeyLength;i++)
   {
      for(j=0;j<KeyLength;j++)
      {
         printf("%3d",table[i][j]);
      }
      printf("\n");
   }
   printf("\n");

}

void fshow(int **table)
{
   int i,j;
   for(i=0;i<KeyLength;i++)
   {
      for(j=0;j<KeyLength;j++)
      {
         fprintf(debug,  "%3d",table[i][j]);
      }
      fprintf(debug, "\n");
   }
}

void ShowDict(char **dict)
{
    int i,j;
    for(i=0;i<items;i++)
    {
        printf("%s\n", dict[i]);
    }
}

//�㭪�� ���६��� ��� ����筮� ��⥬� ��᫥��� �� �।�⠢����� ��⮢
//����⠬� ���ᨢ� char *a. ����訩 ��� ����� �������訩 ������ � ���ᨢ�
//�� ������������� ��९������� �����頥� 0, � ��⠫��� ����� 1.
int add(char *a, int len)
{
    int cr=1; //ᨣ��� ��७��
    len--;
    while(len>=0 && cr)
    {
        if(!a[len])
        {
            a[len]=1;
            cr=0;
        }
        else a[len]=0;
        len--;
    }
    if(len<0 && cr) return 0;
    return 1;
}
