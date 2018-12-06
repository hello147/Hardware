
#include "stm32f10x.h"
#include "string.h"


extern	char content[];
char httpmothd[24]={"post"};		
//char httpurl[156]={"/api/todo"}; 		
char httpaccept[48]={"accept: application/json"}; 		
char httptype[100]={"Content-Type: application/json-patch+json"};
char httptext[100]={"{ \"id\": 13,  \"name\": \"abc\",  \"isComplete\": \"zzzz\"}"};
	extern char content[4096];
extern char senddata[200];

/*  函数名：esp_post
** 
**   post请求帧打包
**     
**   
*/

void esp_post(char *ip,int port,char *page,char *msg)
{
    char content_page[50];
    sprintf(content_page,"POST %s HTTP/1.1\r\n",page);
    char content_host[50];
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);
    char content_type[] = "Content-Type:application/json-patch+json\r\n";
    char content_len[300];
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(msg));
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,msg);
   
}











