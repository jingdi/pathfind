/*
Copyright (c) 2012-2014 streestone.cn and chunbai.com

www.streestone.cn 
www.chunbai.com
email:zhanglei0321@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include "datatype.h"
#include "heap.h"
#include "triangle.h"
#include "navmesh.h"

//for test 
int main(int argc,char **argv)
{
	char *filePath = argv[1];
	char *resultPath = argv[2];
	int fileLen;
	FILE *fp,*fresult;
	char *pData;
	char *pMeshData;
	int iMeshLen = 0;
	int loop = 10;

	fp = fopen(filePath,"rb");
	if(fp == NULL)
	{
		printf("open file %s error\n",filePath);
		//system("pause");
		return 0;
	}
	fresult = fopen(resultPath,"wb");
	if(fresult == NULL)
	{
		printf("open result file error\n",filePath);
		system("pause");
		return 0;
	}
	fseek(fp,0L,SEEK_END);
    fileLen = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	pData = (char *)malloc(fileLen*sizeof(char));
	fread(pData,1,fileLen,fp);
	fclose(fp);

	pMeshData = initData(pData,&iMeshLen);

	srand((int)time(0));
	while(loop)
	{
		 int iSX,iSY,iEX,iEY;
		 Point *pResult = 0;
		 Point *pResult1 = 0;
		 int resultLen = 0;
		 int resultLen1 = 0;
		 iSX =  rand()%2000;
		 iSY =  rand()%1000;
		 iEX =  rand()%2000;
		 iEY =  rand()%1000;
		 if(pathFind(pMeshData,iSX,iSY,iEX,iEY,&pResult,&resultLen)==SUCCESS)
		 {
			 int i;
			 fwrite(&resultLen,sizeof(int),1,fresult);
			 for(i = 0;i<resultLen-1;i++)
			 {
				printf("{%d,%d}->",pResult[i].iX,pResult[i].iY);
				fwrite(&pResult[i].iX,sizeof(int),1,fresult);
				fwrite(&pResult[i].iY,sizeof(int),1,fresult);
			 }
			 printf("{%d,%d}\n",pResult[i].iX,pResult[i].iY);
			 fwrite(&pResult[i].iX,sizeof(int),1,fresult);
			 fwrite(&pResult[i].iY,sizeof(int),1,fresult);
			 free(pResult);
			 loop--;
		 }
		 else
		 {
			 printf("{%d,%d}=====/====={%d,%d}\n",iSX,iSY,iEX,iEY);
		 }
		 _sleep(10);
	}
	fclose(fresult);
	//system("pause");
	return 0;
}