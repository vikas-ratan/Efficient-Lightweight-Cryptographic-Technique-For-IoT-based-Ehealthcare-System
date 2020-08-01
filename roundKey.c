#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
char toHexArray[16][5];
void increment(char code[]){
    for(int i=3;i>=0;i--){
        if(code[i]=='0'){
            code[i]='1';
            break;
        }
        else if(code[i]=='1'){
            code[i]='0';
        }
    }
}
int toInt(char c){
    if((int)c>=48 && (int)c<=57){
        return (int)c-48;
    }
    else{
        return (int)c-97+10;
    }
}
void initialiseHexArray(){
    char code[4]="0000";
    for(int i=0;i<16;i++){
        for(int j=0;j<4;j++){
            toHexArray[i][j]=code[j];
        }
        toHexArray[i][4]='\0';
        increment(code);
    }
}
char* toHex(char c){
    if(c>='0' && c<='9'){
        return toHexArray[(int)c-48];;
    }
    else{
        return toHexArray[(int)c-87];
    }
}
void getBinaryKey(char key[],char binaryKey[]){
    initialiseHexArray();
    for(int i=0;i<64;i++){
        char* tempKey=toHex(key[i]);
        strcat(binaryKey,tempKey);
    }
    binaryKey[256]='\0';
}
void verticalShift(char partKey[][8]){
    int temp[8];
    for(int i=0;i<8;i++){
        temp[i]=partKey[7][i];
    }
    for(int i=6;i>=0;i--){
        for(int j=0;j<8;j++){
            partKey[i+1][j]=partKey[i][j];
        }
    }
    for(int i=0;i<8;i++){
        partKey[0][i]=temp[i];
    }
}
void horizontalShift(char partKey[][8]){
    int temp[8];
    for(int i=0;i<8;i++){
        temp[i]=partKey[i][7];
    }
    for(int i=6;i>=0;i--){
        for(int j=0;j<8;j++){
            partKey[j][i+1]=partKey[j][i];
        }
    }
    for(int i=0;i<8;i++){
        partKey[i][0]=temp[i];
    }
}
void rotation(char partKey[][8],char code[]){
    if(code[0]=='1')
        horizontalShift(partKey);
    if(code[1]=='1')
        verticalShift(partKey);
    if(code[2]=='1')
        horizontalShift(partKey);
    if(code[3]=='1')
        verticalShift(partKey);
}
void Xor(char partKey1[][8],char partKey2[][8],char generatedKey[]){
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(partKey1[i][j]==partKey2[i][j])
                strcat(generatedKey,"0");
            else
                strcat(generatedKey,"1");
        }
    }
}
void stringXor(char s1[],char s2[],char xor[]){
    for(int i=0;i<64;i++){
        if(s1[i]==s2[i])
            xor[i]='0';
        else
            xor[i]='1';
    }
}
void roundKeyGeneration(char key[],char k1[],char k2[]){
    char partKey[4][8][8];//for dividing key into four parts;
    int keyIndex=0;
    for(int i=0;i<4;i++){
        for(int j=0;j<8;j++){
            for(int k=0;k<8;k++){
                partKey[i][j][k]=key[keyIndex];
                keyIndex++;
            }
        }
    }
    for(int i=0;i<4;i++){
        char code[4]="1000";
        for(int j=0;j<16;j++){
            rotation(partKey[i],code);
            increment(code);
        }
    }
    Xor(partKey[0],partKey[1],k1);
    Xor(partKey[2],partKey[3],k2);
}
void additionModulo(char s1[],char s2[],char sum[]){
    char carry='0';
    for(int i=63;i>=0;i--){
        if(carry=='1'){
            if(s1[i]=='1' && s2[i]=='1'){
                sum[i]='1';
                carry='1';
            }
            else if(s1[i]!=s2[i]){
                sum[i]='0';
                carry='1';
            }
            else{
                sum[i]='1';
                carry='0';
            }
        }
        else{
            if(s1[i]=='1' && s2[i]=='1'){
                sum[i]='0';
                carry='1';
            }
            else if(s1[i]!=s2[i]){
                sum[i]='1';
                carry='0';
            }
            else{
                sum[i]='0';
                carry='0';
            }
        }
    }
}
void eachRound(char message[],char k1[],char k2[]){
    char partMsg[4][32],expandMsg[4][64];
    for(int i=0;i<4;i++){
        for(int j=0;j<32;j++){
            partMsg[i][j]=message[i*32+j];
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<16;j++){
            expandMsg[i][j]=partMsg[i][j+16];
        }
        for(int j=0;j<32;j++){
            if(partMsg[i][j]=='0')
                expandMsg[i][j+16]='1';
            else
                expandMsg[i][j+16]='0';
        }
        for(int j=0;j<16;j++){
            expandMsg[i][j+48]=partMsg[i][j];
        }
    }
    char sum[2][64];
    additionModulo(expandMsg[0],expandMsg[1],sum[0]);
    additionModulo(expandMsg[2],expandMsg[3],sum[1]);
    stringXor(sum[0],k1,sum[0]);
    stringXor(sum[1],k2,sum[1]);
    for(int i=0;i<2;i++){
        for(int j=0;j<64;j++){
            message[i*64+j]=sum[i][j];
        }
    }
}
void encryption(char message[],char k1[],char k2[]){
    for(int i=0;i<32;i++){
        eachRound(message,k1,k2);
    }
}
int main(){
    char key[]="1f1e1d1e1b1a191817161514131211100f0e0d0cab0a09080706750403020100";
    double timeElapsed=0.0;
    time_t begin=time(NULL);
    char binaryKey[257]="\0";
    getBinaryKey(key,binaryKey);
    char k1[65]="\0",k2[65]="\0";
    roundKeyGeneration(binaryKey,k1,k2);
    printf("%s\n%s\n",k1,k2);
    char message[32]="";
    printf("enter the messages : ");
    for (int i=0;i<32;i++){
        scanf("%c",&message[i]);
    }
    encryption(message,k1,k2);
    time_t end=time(NULL);
    printf("time elapsed : %f\n",(end-begin)*1000);
    printf("encoded message : %s\n",message);
    return 0;
}