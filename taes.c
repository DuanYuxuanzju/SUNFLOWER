#include <stdio.h>
#include <string.h>

void aes_encrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key);
void aes_decrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key);
void ecb_aes_encrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key);
void ecb_aes_decrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key, int len);
void AddRoundKey(unsigned char *p, unsigned char *k);
void ByteSub(unsigned char *p, int n);
void InvByteSub(unsigned char *p, int n);
void ShiftRow(unsigned char p[4][4]);
void InvShiftRow(unsigned char p[4][4]);
void MixColumn(unsigned char *p, unsigned char *a);
void aes_mul(unsigned char *a,unsigned char *b,unsigned char *c);
unsigned int aes_8_bit_mul(unsigned int x, unsigned int y);
int aes_set_key(unsigned char *key_seed, int key_size, unsigned char *roundkey);

int keyrounds=0;
static const unsigned char sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const unsigned char rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

void aes_encrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key)
{
	int i,j,k;
	unsigned char matrix[4][4];
	unsigned char a[4]={0x02, 0x03, 0x01, 0x01};
	int h;
	/* copy the plaintext into the matrix(the transform of the actual one)*/
	memcpy(matrix,plain,4*4);
	AddRoundKey((unsigned char *)matrix,key);	/* key is array of all the round keys*/
	for(i=1;i<=keyrounds;i++)
	{
		ByteSub((unsigned char *)matrix, 16);
		ShiftRow(matrix);
		if(i<keyrounds)
			MixColumn((unsigned char *)matrix,a);
		AddRoundKey((unsigned char *)matrix, key+i*16);
	}
	/* copy the matrix into cipher */
	memcpy(cipher,matrix,4*4);
}

void ecb_aes_encrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key)
{
	int i;
	int len;
	unsigned char p[16],c[16];
	len=strlen(plain)-1;
	if(len%16!=0)
	{
		printf("The length needs to be dividable by 16.\n");
		return;
	}
	else
	{
		for(i=0;i<len/16;i++)
		{
			memcpy(p,plain+i*16,16);
			aes_encrypt(p,c,key);
			memcpy(cipher+i*16,c,16);
		}
	}
}
void aes_decrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key)
{	
	int i,j,k;
	unsigned char matrix[4][4];
	unsigned char a[4]={0x0E, 0x0B, 0x0D, 0x09};
	int h;
	memcpy(matrix,cipher,4*4);
	for(i=keyrounds;i>=1;i--)
	{
		AddRoundKey((unsigned char *)matrix, key+i*16);
		if(i<keyrounds)
			MixColumn((unsigned char *)matrix,a);
		InvShiftRow(matrix);
		InvByteSub((unsigned char *)matrix, 16);
	}
	AddRoundKey((unsigned char *)matrix,key);
	memcpy(plain,matrix,4*4);
}
void ecb_aes_decrypt(unsigned char *plain, unsigned char *cipher, unsigned char *key, int len)
{
	int i;
	unsigned char p[16],c[16];
	for(i=0;i<len/16;i++)
	{
		memcpy(c,cipher+i*16,16);
		aes_decrypt(p,c,key);
		memcpy(plain+i*16,p,16);
	}
}
void AddRoundKey(unsigned char *p, unsigned char *k)
{
	int i;
	for(i=0;i<16;i++)
	{
		p[i]=p[i]^k[i];		/* xor the plaintext with the key*/
	}
}

void ByteSub(unsigned char *p, int n)
{
	int i;
	unsigned char index;
	for(i=0;i<n;i++)
	{
		index=p[i];
		p[i]=sbox[index];
	}	
}

void InvByteSub(unsigned char *p, int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		p[i]=rsbox[p[i]];
	}
}
void ShiftRow(unsigned char p[4][4])
{
	int i;
	unsigned char temp;
	/*Since the matrix is in transform form, the shiftrow is actually shift column*/
	temp=p[0][1];
	p[0][1]=p[1][1];
	p[1][1]=p[2][1];
	p[2][1]=p[3][1];
	p[3][1]=temp;
	
	temp=p[0][2];
	p[0][2]=p[2][2];
	p[2][2]=temp;
	temp=p[1][2];
	p[1][2]=p[3][2];
	p[3][2]=temp;
	
	temp=p[3][3];
	p[3][3]=p[2][3];
	p[2][3]=p[1][3];
	p[1][3]=p[0][3];
	p[0][3]=temp;

}

void InvShiftRow(unsigned char p[4][4])
{
	int i;
	unsigned char temp;
	temp=p[3][1];
	p[3][1]=p[2][1];
	p[2][1]=p[1][1];
	p[1][1]=p[0][1];
	p[0][1]=temp;
	
	temp=p[0][2];
	p[0][2]=p[2][2];
	p[2][2]=temp;
	temp=p[1][2];
	p[1][2]=p[3][2];
	p[3][2]=temp;

	temp=p[0][3];
	p[0][3]=p[1][3];
	p[1][3]=p[2][3];
	p[2][3]=p[3][3];
	p[3][3]=temp;
}

unsigned int aes_8_bit_mul(unsigned int x, unsigned int y)
{
	/* nong fu algorithm */
	int i,j;
	unsigned int ans=0;
	for(i=0;i<8;i++)
	{
		if(y&1)
			ans=ans ^ x;
		y=y>>1;
		x=x<<1;
		if(x & 0x100)
			x = x ^ 0x11B;	
	}
	return ans;
}

void aes_mul(unsigned char a[4],unsigned char b[4],unsigned char c[4])
{
	int i,j;
	unsigned char x[4],y[4];
	unsigned char temp;
	memcpy(y,b,4);
	memset(x,0,4);
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			x[i]=x[i]^(aes_8_bit_mul(y[j],a[j]));
		temp=y[3];
		y[3]=y[2];
		y[2]=y[1];
		y[1]=y[0];
		y[0]=temp;
	}
	memcpy(c,x,4);
}

void MixColumn(unsigned char *p, unsigned char *a)
{
	unsigned char temp[4];
	int i;
	for(i=0;i<4;i++)
	{
		memcpy(temp,p+i*4,4);
		aes_mul(temp,a,temp);
		memcpy(p+i*4,temp,4);
	}
}

int aes_set_key(unsigned char *key_seed, int key_size, unsigned char *roundkey)
{
	unsigned int r;
	int i,j,h;
	int step,loop;
	unsigned char temp;
	unsigned char pk[4];
	unsigned int p;
	switch(key_size)
	{
		case 128:keyrounds=10;step=4;loop=10;break;
		case 192:keyrounds=12;step=6;loop=8;break;
		case 256:keyrounds=14;step=8;loop=7;break;
		default:keyrounds=0;step=0;loop=0;return 0;break;
	}
	memcpy(roundkey,key_seed,key_size/8);	/* the first round key is key sed*/
	r = 1;
	for(i=step;i<step+loop*step;i+=step)
	{
		p=i*4;
		memcpy(roundkey+p,roundkey+p-4,4);	/* rk[i*4]=rk[i*4-1]*/
		temp=roundkey[p];
		roundkey[p]=roundkey[p+1];
		roundkey[p+1]=roundkey[p+2];
		roundkey[p+2]=roundkey[p+3];		
		roundkey[p+3]=temp;			/* rol 1 */
		ByteSub(roundkey+p,4);		/* Byte Sub */
		if(r > 0x80)
			r = r ^ 0x11B;
		roundkey[p] = roundkey[p] ^ r;
		r=r<<1;
		for(j=0;j<4;j++)
			roundkey[p+j]=roundkey[p+j]^roundkey[p-step*4+j];
		for(j=1;j<step;j++)
		{
			if((step==6 && i==step*loop && j>=step-2) ||
			(step==8 && i==step*loop && j>=step-4))
				break;

			if(step==8 && j==4)
			{
				memcpy(pk,roundkey+p+3*4,4);
				ByteSub((unsigned char*)pk,4);
				for(h=0;h<4;h++)
					roundkey[p+4*4+h]=pk[h]^roundkey[p+4*4-step*4+h];
			}
			else
			{
				for(h=0;h<4;h++)
					roundkey[p+j*4+h]=roundkey[p+j*4-4+h]^roundkey[p+j*4-step*4+h];
			}
		}
	}
	return 1;	
}

int main()
{
	unsigned char key_seed[100]="0123456789ABCDEF0123456789ABCDEF";
	unsigned char roundkey[240];	/* most: 256bit 14rounds (14+1)*16 */
	unsigned char plaintext[100];
	unsigned char ciphertext[100];
	unsigned char buf[100];
	int key_size=256;
	int i;
	int len;
	int choice;
	char ch;
	aes_set_key(key_seed,key_size,roundkey);
	printf("What do you want?\n1)Encrypt a message\n2)I want the key!\n");
	scanf("%d",&choice);
	if(choice==1)
	{
		printf("Please input the message:\n");
		while((ch=getchar())!='\n' && ch!=EOF);
		fgets(plaintext,100,stdin);
		printf("The plaintext is: %s",plaintext);
		len=strlen(plaintext)-1;
		ecb_aes_encrypt(plaintext,ciphertext,roundkey);
		printf("The cipher is: ");
		for(i=0;i<16;i++)
			printf("%02X",ciphertext[i]);
		printf("\n");
		//ecb_aes_decrypt(buf,ciphertext,roundkey,len);
		//buf[len]=0;
		//printf("%s\n",buf);
	}
	else
	{
		printf("The key is:\n");
		for(i=0;i<32;i++)
			printf("%02X",key_seed[i]);
		printf("\n");
	}
	return 0;
}
