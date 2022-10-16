/* Программа для расшифровки сообщений, закодированных с помощью квадрата 
Виженера. Использует перебор по словарю. Компилируется BCС5.02 Win32 only.
Для переноса на gcc необходимо переписать все функции, работающие с плавучкой и
работу со словарем, там что-то не совсем так.
Присутствуют множественные утечки памяти!!!
Авторы: Анатолий Савченков
	Роман Головин
	Кочетков Владимир
	Стрижкин Андрей
	Харин Игорь
	Бесстрашнова Анна
	Пономарев Константин
Санкт-Петербургский Государственный Технический Университет
Кафедра ИБКС группа 1088/2
1999г.
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
#define ALPHABET_LENGTH 32                              //Длина алфавита.

/*===========================================================================*/
typedef struct queue
{
    char *data;
    struct queue *p;
} queue;

/*----------------------Глобальные переменные--------------------------------*/

int    KeyLength;    //Длина ключевого слова.
FILE   *debug;       //Если определена, выводятся все промежуточные результаты.
//массив частот появления русских букв объявляется глобально под именем float rus[32]

char *mem_err="Недостаточно памяти!\n";
long items;
queue *top=NULL;
queue *bottom=NULL;
/*===========================================================================*/

/*===========================================================================*/

void main(int argc, char *argv[])
{
    char  *   InString,                                //Зашифрованный текст.
          *   OutString,                               //Расшифрованный текст.
          *   ReplaceMatrix,                           //Матрица перестановок.
          *   KeyWord;                                 //Ключевое слово.
    int   **  Groups,                                  //матрица групп.

          *   Sdvigs;                                  //массив сдвигов между группами.

    float *** Pklr,
          *** PkXiYj;
    int ***ConvertedPklr;

    printf("Санкт-Петербургский Государственный Технический Университет\n\
Кафедра ИБКС группа 1088/2, 1999 г.\n\
Дешифровщик текстов, зашифрованных по алгоритму Виженера, вер. 1.0\n\
(с) Анатолий Савченков, Роман Головин, Кочетков Владимир, Стрижкин Андрей,\n\
Харин Игорь, Бесстрашнова Анна, Пономарев Константин, 1999-2000\n");	

    if(argc<3)
    {
        printf("Использование: VIGINER входной_файл выходной_файл [расширенный_отчет]\n");
        return;
    }
    if(argc>=4 && (debug=fopen(argv[3],"a")))
    {
        printf("Расширенный отчет записывается в файл: %s\n",argv[3]);
    }

    InString = tdpReadInputFile(argv[1]);

    KeyLength = andreyKeyWordLength(InString);

    Groups = tdpGroupsFromFile (argv[1]);

    PkXiYj = tolyaPkXiYj(Groups);

    Pklr = vovaPklr(PkXiYj);

    ConvertedPklr=ConvertPklr(Pklr);

    if(debug)
    {
        fprintf(debug, "Матрица ConvertedPklr:\n");
        tdpShow3DMatrix(ConvertedPklr,KeyLength,KeyLength,ALPHABET_LENGTH);
    }

    //Keyword search using dictionary attack.

    enumerate(ConvertedPklr);

    if((KeyWord=(char *)calloc(KeyLength+1,sizeof(char)))==NULL)
    {
      printf(mem_err);
      exit(1);
    }

    printf("Введите подходящее ключевое слово(если невозможно выбрать ключевое\nслово попробуйте более глубокий или менее строкий поиск): ");
    fflush(stdin);
    fscanf(stdin, "%s", KeyWord);
    //fgets(KeyWord,KeyLength+1,stdin);
    if(!KeyWord[0]) exit(1);

    if(debug) fprintf(debug, "\n\nКлючевое слово: %s\n\n\n", KeyWord);

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

    printf("Готово!!!");

    free(KeyWord);

    if(debug)
    {
        fprintf(debug, "\nЗавершаем выполнение.\n");
        fclose(debug);
    }
}

/*-------------------------Чтение исходного файла в массив-------------------*/

char * tdpReadInputFile(char * FileName)
{
    char *InputArray, ch;
    long NewLength = 0;

    FILE *InputFile;

    printf("Загружаем текст из файла %s...",FileName);

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
        if((ch >= 'А') && (ch <= 'Я'))
        {
            InputArray[NewLength++] = ch;
        }
    }

    InputArray = (char*)realloc(InputArray, (NewLength + 1));

    fclose(InputFile);

    printf(" готово.\n\n");

    if(debug)
    {
        fprintf(debug,"\nОбрабатываем файл %s\n%s\n\n",FileName,InputArray);
    }

    return(InputArray);
}

/*-----------------------Вычисление длины ключевого слова--------------------*/

int andreyKeyWordLength(char *Text)
{
   float SrIS,IS;
   int TextLength=strlen(Text);
   int *KeyLength;
   int i,j,k,Cislo;
   int Matr[33];
   int count = 0;
   printf("Вычисляем длину ключевого слова...\n");
   if(debug) fprintf(debug,"Вычисляем длину ключевого слова:\n");
   /*Выделение памяти под массив.*/
   if ((KeyLength = (int*)calloc(20, sizeof(int))) == NULL)
   {
        printf(mem_err);
        exit(1);
   }
   for(i=1;i<=20;i++)    //Предполагаем длину слова "i" от 1 до 20
   {
      SrIS=0;
      for(j=0;j<=i-1;j++)//Проходим по всем подгруппам текста(их всего i шт.)
      {
         for (k=0;k<=32;k++) Matr[k]=0;
         for(k=0;;k++)//Перебираем все буквы стоящие на нужных позициях(поз. j+i*k)
         {
            if(j+k*i>=TextLength)break;//Условие прекращения перебора букв
            //Из всех int'овых эквивалентов букв вычитаем "А"(0x80)
            Matr[*(Text+j+i*k)-'А']++; // и получаем число от 0 до 32
          }
          Cislo=k;
          IS=0;
          for(k=0;k<=32;k++) IS+=((float)Matr[k]*((float)Matr[k]-(float)1))/((float)Cislo*((float)Cislo-(float)1));//Высчитываем индекс совпадения
          SrIS+=IS/(float)i;//Высчитываем средний индекс совпадения(это отсебячина ,облегчающая жизнь) При каждом j он увеличивается и к j=i максимален и является действительно средним И.С.
      }
      if(SrIS>=0.045&&SrIS<=0.065)
      {
        //Заполняем матрицу и экран подходящими ср. И.С.(SrIS)
        printf("%d. Длина ключевого слова=%2d, индекс совпадения=%.3f\n",count+1,i,SrIS);
        *(KeyLength + count++) = i;
      }
      if(debug)
      {
        fprintf(debug,"Длина ключевого слова=%2d, индекс совпадения=%.3f\n",i,SrIS);
      }
   }
   do
   {
     printf("Введите номер наиболее подходящей длины ключевого слова,\nосновываясь на индексе совпадение (теоретическое значеие равно 0.055) ");
     scanf("%d",&j);
     j--;
   }
   while(j>=count);
   printf("Длина ключевого слова равна %d.\n\n", *(KeyLength+j));
   if(debug) fprintf(debug,"Длина ключевого слова равна %d.\n\n", *(KeyLength+j));

   i=*(KeyLength+j);
    free(KeyLength);
   return i;
}

/*-------------------Заполнение групп из файла-----------------------*/

int ** tdpGroupsFromFile(char * FileName)//2-ая функция- разбиение на группы
{
    int **Groups, i, j;
    char ch;
    FILE *InputFile;
    printf("Создаем группы...");
    if((InputFile = fopen(FileName, "rb")) == NULL)
    {
        perror(FileName);
        exit(1);
    }

    /*Выделение памяти под матрицу Groups[KL][AL].*/
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
    // подсчет результатов G(ij)
    while((ch = getc(InputFile)) != EOF)
    {
        if(i == KeyLength)
            i = 0;

        if((ch >= 'А') && (ch <= 'Я'))
        {
            Groups[i++][ch -'А']++;
        }
    }

    fclose(InputFile);

    printf(" готово.\n\n");

    if(debug)
    {
        fprintf(debug,"Создаем группы:\n");
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
        fprintf(debug,"%3c", col + 'А');
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

/*--------------Вычисление PkXiYj и заполнение выходной матрицы---------------*/

float ***tolyaPkXiYj(int **Groups)
{
   float ***PkXiYj;

   long double SumPkYjXm;
   int k, i, j, m, x, y; //Nk-число букв в к-ой группе

   printf("Вычисляем матрицу PkXiYj...");

   //Отводим память под матрицу размером KxN1xN2, где K - число групп;
        //N1 и N2 - длина алфавита,
   //Координата N1 соответствует буквам реального алфавита, а
   //Координата N2 соответствует буквам шифрованного алфавита.
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

   //Заполняем выходрую матрицу
   for (k=0; k<KeyLength;k++)
   {
      //Пробегаем все буквы реального алфавита
      for (i=0; i<ALPHABET_LENGTH; i++)
      {
        //Пробегаем все буквы k-ой группы шифрованного алфавита
         for (j=0; j<ALPHABET_LENGTH; j++)
         {
            //Вычисляем сумму PkYjXm для 1<=m<=ALPHABET_LENGTH
            SumPkYjXm=0;
            for (m=0;m<ALPHABET_LENGTH; m++)
                SumPkYjXm+=tolyaPkYjXi(Groups, k, j, m);
            //Вычисляем и заносим в матрицу значение PkXiYj для текущих k, i, и j.
            PkXiYj[k][i][j]=(float)(tolyaPkYjXi(Groups, k, j, i)/SumPkYjXm);
         }
      }
   }
   printf(" готово.\n\n");

   if(debug)
   {
        fprintf(debug, "Вычисляем матрицу PkXiYj:\n");
        ShowPkXiYjMatrix(PkXiYj,KeyLength,32,32);
   }

   return PkXiYj;
}

/*------------------Вычисление Pk(Yj|Xi) Вспомогательная функция--------------*/

//Nk - общее число букв в k-ой группе
//Fkj - число появлений j-ой буквы в k-ой группе
//Pi - средняя частота появления i-ой буквы в русском алфавите(берется из массива
//float rus[ALPHABET_LENGTH];)
long double tolyaPkYjXi(int **Groups, int k, int j, int i)
{
   int Nk=0,n;
   long double PkYjXi;

   //Подсчтываем Nk для (k-1)-ой группы
   for (n=0; n<ALPHABET_LENGTH; n++)
      Nk+=Groups[k][n];

        PkYjXi=tdpCnk(Groups[k][j], Nk)*powl(rus[i],Groups[k][j])*powl(1.0-rus[i],(Nk-Groups[k][j]));
   return PkYjXi;
}

/*--------------Вычисление С из N по К (вспомогательная функция)-------------*/

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

/*--------Матрица вероятностей сдвигов r между группами k и l (Pklr)---------*/

float*** vovaPklr (float ***PkXiYj)
{
    int k, l, r, x, y;
    float ***Pklr;

    printf("Вычисляем матрицу Pklr...");

    //Выделение памяти под Pklr.
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

    /*Освобождение PkXiYj(PkXiYj)*/
    for(x = 0; x < KeyLength; x++)
    {
        for(y = 0; y < ALPHABET_LENGTH; y++)
        {
            free(PkXiYj[x][y]);
        }
        free(PkXiYj[x]);
    }
    free(PkXiYj);

    printf(" готово.\n\n");

    if(debug)
    {
        fprintf(debug,"Вычисляем матрицу Pklr:\n");
        ShowPkXiYjMatrix(Pklr,KeyLength,KeyLength,32);
    }
    return Pklr;
}

/*------Считает промежуточные велечины Pklr (Вспомогательная функция)--------*/

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

/*Считает знаменатель второй формулы: сумма по r Pklr(Вспомогательная функция)*/

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



/*---------Нахождение комбинации сдвигов с максимальной вероятностью---------*/

int * tdpSdvigTable(float *** pklr)
{
    int * SdvArr, * SdvArrCopy;

    printf("Создаем матрицу SdvigTable.\n(Подождите пожалуйста, это может занять много времени)...\n");

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

    printf(" готово.\n\n");

    return(SdvArrCopy);
}

/*------------Вспомогательная функция для tdpSdvigTable----------------------*/

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


/*-----------------------Подбор ключевого слова------------------------------*/

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
            printf("%c", ('А' + (FirstLetter - Sdvigs[group] + ALPHABET_LENGTH)%ALPHABET_LENGTH));
        }

    }

    do
    {
        printf("\n Введите номер КС.\n");
        scanf("%d",&FirstLetter);
    }while((FirstLetter < 0) || (FirstLetter >= ALPHABET_LENGTH));

    for(group = 0; group < KeyLength; group++)
    {
        KeyWord[group] = 'А' + (FirstLetter - Sdvigs[group] + ALPHABET_LENGTH)%ALPHABET_LENGTH;
    }
    KeyWord[KeyLength] = '\0';

    return KeyWord;
}


//--------------------------Уточнение сдвигов--------------------------------//

void tdpCorrectSdvigTable(int * Sdvigs, char * KeyWord)
{
    int number = -1, delta;
    char ch;

    clrscr();

    do
    {
        printf("Ключевое слово: %s\n", KeyWord);

        printf("\n\nКакую по порядку букву следует исправить?\nЕсли все правильно, введите 0\n");
        scanf("\n%d", &number);
        if((number < 1) || (number > KeyLength))
        {
            break;
            //return;
        }
        else
        {
            number--;
            printf("\n\nНа какую букву ее следует заменить?\n");
            scanf("\n%c", &ch);

            delta = KeyWord[number] - ch;
            KeyWord[number] = ch;
            Sdvigs[number] = (Sdvigs[number] + delta + ALPHABET_LENGTH) % ALPHABET_LENGTH;
        }
    }while(1);

    if(debug)
    {
        fprintf(debug,"Таблица сдвигов между буквами ключевого слова:\n");
        tdpShow1DMatrix(Sdvigs, KeyLength);
        fprintf(debug, "Ключевое слово: %s\n\n", KeyWord);
    }

    return;
}



/*-------------Составление массива перестановок (вариант 3)------------------*/

char * tdpReplaceMatrix3(long double **M)
{
    char *ReplaceMatrix, *used_i;
    int count, i, j, max_i = 0, max_j = 0;
    long double max = -1e1000;

    printf("Создаем матрицу перестановок(ReplaseMatrix)...");

    //Выделение памяти под массивы.
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
    printf(" готово.\n\n");

    return (ReplaceMatrix);
}


/*-Составление матрицы вероятностей совпадения букв зашифр. и исходн. текстов*/

long double ** tdpPXiYjMatrix(int **Groups, int *r)
{
    int i, j, m;
    long double **M;
    long double sum = 0.0;

    //Выделение памяти под матрицу.
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


    //Собственно, подсчет p(Xi|Yj) и занесение этих вероятностей в матрицу.
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

    //Освобождаем Groups
    for(i = 0; i < KeyLength; i++)
    {
        free(Groups[i]);
    }

    free(Groups);
    if(debug)
    {
        fprintf(debug,"Матрица вероятостей совпадения букв шифрованного и реального алфавитов PXiYj:\n");
        ShowPXiYjMatrix(M,32,32);
    }
    return M;
}

/*----------------Вспомогательная функция для tdpPXiYjMatrix----------------*/

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


/*-------------------Вывод расшифрованного текста в файл---------------------*/

void tdpOutToFile(char * InFileName, char * OutFileName, char *ReplaceMatrix, int *r)
{
    int count = 0;
    char in_char, out_char;
    FILE * OutFile, * InFile;

    printf("Расшифровываем файл %s в файл %s...", InFileName, OutFileName);

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
        if((in_char >= 'А') && (in_char <= 'Я'))
        {
            out_char = ((ReplaceMatrix[in_char - 'А'] + r[count % KeyLength]) % ALPHABET_LENGTH + 'А');
            fputc(out_char, OutFile);
            count++;
        }
    }
    free(ReplaceMatrix);
    free(r);
    printf(" готово.\n\n");
}

/*------------------Вывод расшифрованного текста в массив--------------------*/

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
//        OutArray[count] = (ReplaceMatrix[(String[count] - 'А' - r[count % KeyLength] + ALPHABET_LENGTH) % ALPHABET_LENGTH] + 'А');
        OutArray[count] = ((ReplaceMatrix[String[count] - 'А'] + r[count % KeyLength]) % ALPHABET_LENGTH + 'А');
    }
    OutArray[count] = '\0';

    return (OutArray);
}

/*-----------------------Уточнение ReplaceMatrix-----------------------------*/

char * tdpCorrectReplaceMatrix(char * In, char * Out, char * RM, int * r, char *KeyWord)
{
    char ch;//Изменяемый символ.
    int n;//Номер обрабатываемой буквы в массиве.

    while(1)
    {
        clrscr();

        tdpShowDecryptedText(Out);

        do
        {
            printf("\nНужны ли исправления? (Д/Н)(%f): \n",IsRussianText(Out,strlen(Out)));
            scanf("\n%c",&ch);
        }while((ch != 'Д') && (ch != 'Н'));

        if(ch == 'Н')
            break;

        do
        {
            printf("\nВведите номер буквы:\n");
            scanf("%d",&n);
        }while((n < 0) || (n >= SIZE_OF_OUT_ARRAY));

        do
        {
            printf("\nЗаменить букву %c на:\n", Out[n]);
            scanf("\n%c",&ch);
        }while((ch < 'А') || (ch > 'Я'));

        RM[In[n] - 'А'] = (ch - 'А' - r[n % KeyLength] + ALPHABET_LENGTH) % ALPHABET_LENGTH;

        Out = tdpOutToArray(In, RM, r);
    }
    if(debug)
    {
        char *Viz;
        int count;
        fprintf(debug,"\nПерестановочная матрица:\n");
        tdpShow1DCharMatrix(RM, ALPHABET_LENGTH);
        if ((Viz = (char*)calloc(ALPHABET_LENGTH, sizeof(char))) == NULL)
        {
            printf(mem_err);
            return RM;
        }
        fprintf(debug,"Исходный квадрат Виженера:\n");
        //Получаем первую строку квадрата Виженера
        for(n=0;n<ALPHABET_LENGTH;n++)
        {
            Viz[(RM[n]+(KeyWord[0]-'А'))%ALPHABET_LENGTH]=n+'А';
        }

        //Выводим весь квадрат
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

//Циклический сдвиг элементов массива вправо
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

//Циклический сдвиг элементов массива влево
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
/*--------------------Переводит ключевое слово в сдвиги----------------------*/
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

/*=========================Ромкины функции===================================*/



/*-----------------------Предыдущая, но с -log  -----------------------------*/

long ** tdpPXiYjMatrix2(int **Groups, int *r)
{
    int i, j, m;
    long **M;
    long double sum = 0.0;

    //Выделение памяти под матрицу.
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


    //Собственно, подсчет p(Xi|Yj) и занесение этих вероятностей в матрицу.
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

    //Освобождаем Groups (from tolyaPkXiYj)
    for(i = 0; i < KeyLength; i++)
    {
        free(Groups[i]);
    }

    free(Groups);
//    ShowPXiYjMatrix(M,32,32);
//    fflush(stdout);

    return M;
}


/*-------------Составление массива перестановок (вариант 1)------------------*/

char * tdpReplaceMatrix(long double **M)
{
    char *ReplaceMatrix, *used_i;
    int i, j, max_i = 0, max_j = 0;

    //Выделение памяти под массивы.
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


    //Составление массива перестановок.
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

    //Освобождение used_i и M(PXiYj).
//    free(used_i);

    for(i = 0; i < ALPHABET_LENGTH; i++)
    {
        free(M[i]);
    }

    free(M);


    return ReplaceMatrix;
}

/*-------------Составление массива перестановок (вариант 2)------------------*/

char * tdpReplaceMatrix2(long **M)
{
    char *ReplaceMatrix, *TmpRepM;
    int i;

    //Выделение памяти под массивы.
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

    //Освобождение M(PXiYj).

    for(i = 0; i < ALPHABET_LENGTH; i++)
    {
        free(M[i]);
    }

    free(M);


    return ReplaceMatrix;
}

/*------------------Вспомогательная для tdpReplaceMatrix2--------------------*/

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

/*-------------Составление массива перестановок (вариант 4)------------------*/

char * tdpReplaceMatrix4(int ** Groups, int * Sdvigs)
{
    float Raspred[ALPHABET_LENGTH], max_freq;
    long N = 0, n;
    int group, letter, count, x, max_x;
    char rus_sorted[ALPHABET_LENGTH] = {'О', 'А', 'Е', 'И', 'Н', 'Т', 'Р', 'С',
                                        'П', 'М', 'В', 'Л', 'К', 'Д', 'Я', 'Ы',
                                        'Б', 'З', 'У', 'Г', 'Ь', 'Ч', 'Й', 'Х',
                                        'Ц', 'Ж', 'Ю', 'Щ', 'Ф', 'Э', 'Ш', 'Ъ'};
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

        ReplaceMatrix[max_x] = rus_sorted[count] - 'А';
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

/*-------------------Нахождение сдвигов по 1 части книги---------------------*/

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

//Вспомогательная функция (для предыдущей).
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

/*---------------Преобразование массива сдвигов в матрицу--------------------*/

int ** tdpSdvigMatrix(int * SdvArr)
{
    int ** SdvigTable;
    int k, l;

   /*Выделение памяти под матрицу.*/
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

    // Составление таблицы сдвигов из сдвигов относительно 1-й буквы.
    for(k = 0; k < KeyLength; k++)
    {
        for(l = 0; l < KeyLength; l++)
        {
            SdvigTable[k][l] = (SdvArr[l] - SdvArr[k] + ALPHABET_LENGTH) % ALPHABET_LENGTH;
        }
    }

    return (SdvigTable);
}



//=============================МУСОР=========================================//

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

/*==========================Функции Игоря====================================*/

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
/*==========================Вовкины функции==================================*/
/*===========================================================================*/
// Блок второй функции
// Составление линейной матрицы, в которой по порядку следования идет
// величина сдвигов для фиксированной клетки k*l

///////////////////////////////////////////////////////////////////////////////
// Ищет максимальную вероятность и записывает туда 0, после чего возвращает
// индекс элемента

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
// Непосредственно заполняет линейную матрицу типа int*

int* vovaConvert (float ***matrix, int k, int l)
{
    int *convertedliniarmatrix, i;
    // int convertedliniarmatrix[ALPHABET_LENGTH];

    float *KL_Line;
    //Выделение памяти под convertedliniarmatrix
    if ((convertedliniarmatrix = (int*)calloc(ALPHABET_LENGTH, sizeof(int))) == NULL)
    {
        printf(mem_err);
        exit(1);
    }
    //Выделение памяти под KL_Line
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

/*==========================Толины функции===================================*/
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
//Конвертирует матрицу вероятностей Pklr в матрицу сдвигов
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
    //Выделение памяти под KL_Line
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
    
    printf("\nЗагружаем словарь...");
    if((handle=_rtl_open((const char*)itoa(KeyLength,buff,10),O_RDONLY|SH_DENYWR))==-1)
    {
        printf(" Словарь не найден!\n");
        return NULL;
    }
    items=filelength(handle)/(KeyLength+1);
    if((dict=(char **)farcalloc(items,sizeof(char *)))==NULL)
    {
        printf(" Невозможно отвести память для словаря!\n");
        return NULL;
    }

    for(i=0;i<items;i++)
    {
        if((dict[i]=(char *)calloc(KeyLength+1,sizeof(char)))==NULL)
        {
            printf(" Невозможно отвести память для словаря!\n");
            for(;i>=0;i--)
            {
                free(dict[i]);
            }
            return NULL;
        }
        _rtl_read(handle,dict[i],KeyLength+1);
    }
    printf(" готово.\n");
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

//функция стравнения строк
int strncomp(const void *str1, const void *str2)
{
    int i=0;
    //printf("Comparing: %s<-->%s(%Fp)\n",str1,str2);
    while(!(*((unsigned char *)str1+i)-*((unsigned char *)str2+i)) && i<(KeyLength-1)) i++;
    return (*((unsigned char *)str1+i)-*((unsigned char *)str2+i));
}

//Бинарный поиск, слова в словаре

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

//Заполняем таблицу на основе первой строки
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

//Обнуляем таблицу
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

//Проверяем рассчитанную таблицу по матрице ConvertedPklr
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

//Перебор ключевых слов на основе ConvertedPklr
//Наиболее вероятные слова проверяются первыми
/* Описалово алгоритма перебора, без 0.5л не разобраться.
Спасибо 2:5030/1158.14!
 Для к от 0 до н-1 (н - число столбцов) в цикле:
  Обрабатываешь все числа, содержащие цифры от 0 до к. То есть числа, состоящие
  из "к" и "?". (При этом, число ??? надо исключить, оно, то есть _они_ на
  самом деле, уже были.) Как это сделать, если к=0 понятно, (это просто 0) а
  если нет? Тогда так.
  Для л от 1 до 10^м-1 (в двоичной системе, то есть до 2^м-1) в цикле:
   Заменяешь в м-значной записи числа л каждый 0 на ?.
   Заменяешь каждую 1 на к.
   В принципе, лучше просто заполнить массив м чисел цифрами к и запомнить
   несколько указателей на те ячейки, где должны быть "?".
   Число "?" (число вопросов) считается по ходу дела и запоминается как р. Если
   оно не ноль, то делай так.
   Для каждого п от 0 до (к-1)^р-1, в (к-1)-ричной системе счисления в цикле:
    Подставляем в "маску" цифры числа п, ведь в этой системе счисления п как
    раз р-значное. Кстати, можешь сразу использовать массив указателей для
    доступа к цифрам числа п.
    Дальше - обрабатываешь полученное по маске число, это и есть очередное xyz.
*/

void enumerate(int ***ConvertedPklr)
{
   int i, j, **table, prec, errors;
   char *a, *index, *sub_index;
   int k, l, p, q;
   long count=0;
   char *data;

   char **dict;

   printf("Сколько наиболее вероятных сдвигов будет использовано\nпри проверки корректности таблицы сдвигов(значение от 1 до 32): ");
   scanf("%d", &prec);
   printf("\nВведите максимально допустимое число несоотвествий между таблицой сдвигов\nи матрицей ConvertedPklr - число возможных ошибок в таблице сдвигов,\nпозволяющее отнести эту таблицу к допустимым\n(имеет смысл вводить числа от 0 до 3, но это не критично): ");
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

   //Загружаем словарь
   if((dict=InitSearch())==NULL)
   {
      //printf("Not enouth memory to load dictionary!\n");
      exit(1);
   }
   //ShowDict(dict);

    //Новый генератор перестановок
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
            //Заполняем значением k и считаем число вопросов
            p=0;
            for(l=KeyLength-1;l>=0;l--)
            {
                if(!a[l]) p++;
                else index[l]=k;
            }

            if(!p)
            {
                //Начало секции
                count++;
                //Заполняем таблицу сдвигов начальными значениями
                for(i=0;i<KeyLength-1;i++)
                {
                    table[i][i+1]=ConvertedPklr[i][i+1][index[i]];
                }
                //Вычисляем таблицу сдвигов целиком
                calculate(table);
                //Проверяем
                if(check(table, ConvertedPklr, prec)<=errors)
                {
                    //Генерим все слова из сдвигов и проверяем по словарю
                    for(i = 0; i < ALPHABET_LENGTH; i++)
                    {
                        for(j = 0; j < KeyLength; j++)
                        {
                            data[j]=0x80 + (i - table[0][j] + ALPHABET_LENGTH)%ALPHABET_LENGTH;
                            data[KeyLength]=0;
                        }
                        if(BinarySearch(dict, data))
                        {
                            printf("Найдено ключевое слово: %s\n",data);
                            if(debug)
                            {
                                fprintf(debug, "-----------------\nНайдено ключевое слово: %s\n", data);
                                fshow(table);
                            }
                        }
                    }
                }
                zero(table);
                //Конец секции
                break;
            }
            if((sub_index=(char *)calloc(p, sizeof(char)))==NULL)
            {
                printf(mem_err);
                exit(1);
            }
            //Заполняем вопросы значениями
            while(sub_index[0]!=k)
            {
                p=0;
                for(l=KeyLength-1;l>=0;l--)
                {
                    if(!a[l]) index[l]=sub_index[p++];
                }
                p--;
                sub_index[p]++;

                //Начало секции
                count++;
                //Заполняем таблицу сдвигов начальными значениями
                for(i=0;i<KeyLength-1;i++)
                {
                    table[i][i+1]=ConvertedPklr[i][i+1][index[i]];
                }
                //Вычисляем таблицу сдвигов целиком
                calculate(table);
                //Проверяем
                if(check(table, ConvertedPklr, prec)<=errors)
                {
                    //Генерим все слова из сдвигов и проверяем по словарю
                    for(i = 0; i < ALPHABET_LENGTH; i++)
                    {
                        for(j = 0; j < KeyLength; j++)
                        {
                            data[j]=0x80 + (i - table[0][j] + ALPHABET_LENGTH)%ALPHABET_LENGTH;
                            data[KeyLength]=0;
                        }
                        if(BinarySearch(dict, data))
                        {
                            printf("Найдено ключевое слово: %s\n",data);
                            if(debug)
                            {
                                fprintf(debug, "-----------------\nНайдено ключевое слово: %s\n", data);
                                fshow(table);
                            }
                        }
                    }
                }
                zero(table);
                //Конец секции

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

//Просматривать слова до удаления словаря!!!(при использовании очереди put, get)

   //Удаляем словарь
   DeInitSearch(dict);
   printf("Обработано вариантов: %d\n", count);
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

//Функция инкремента для двоичной системы счисления при представлении битов
//элементами массива char *a. Младший бит имеет наибольший индекс в массиве
//При возникновении переполнения возвращает 0, в остальных случаях 1.
int add(char *a, int len)
{
    int cr=1; //сигнал переноса
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
