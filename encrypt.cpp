#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//Fixed values during AES encryption
vector<vector<unsigned char>> SBox={
  {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76},
  {0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0},
  {0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15},
  {0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75},
  {0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84},
  {0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF},
  {0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8},
  {0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2},
  {0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73},
  {0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB},
  {0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79},
  {0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08},
  {0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A},
  {0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E},
  {0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF},
  {0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}
};

vector<vector<unsigned char>> InvSBox={
    {0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB},
    {0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB},
    {0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E},
    {0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25},
    {0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92},
    {0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84},
    {0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06},
    {0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B},
    {0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73},
    {0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E},
    {0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B},
    {0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4},
    {0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F},
    {0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF},
    {0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61},
    {0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D}
};

const array<unsigned char, 16> mixColumnsMatrix={
    0x02, 0x03, 0x01, 0x01,
    0x01, 0x02, 0x03, 0x01,
    0x01, 0x01, 0x02, 0x03,
    0x03, 0x01, 0x01, 0x02
};

const array<unsigned char, 16> invMixColumnsMatrix={
    0x0E, 0x0B, 0x0D, 0x09,
    0x09, 0x0E, 0x0B, 0x0D,
    0x0D, 0x09, 0x0E, 0x0B,
    0x0B, 0x0D, 0x09, 0x0E
};

const array<unsigned char, 11> Rcon={
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};


//Key generation and expansion
void generateKey(unsigned char* key, int keySize){
    srand(static_cast<unsigned int>(time(0)));

    for(int i=0;i<keySize;++i){
        key[i] =static_cast<unsigned char>(rand()%256);
    }
}

void RotateWord(unsigned char* word){
    unsigned char temp=word[0];
    word[0]=word[1];
    word[1]=word[2];
    word[2]=word[3];
    word[3]=temp;
}

void ExpandKey(const unsigned char* originalKey,unsigned char* expandedKey) {
    memcpy(expandedKey,originalKey,16);

    unsigned char temp[4];
    int bytesGenerated= 16;

    while (bytesGenerated<176) {
        for (int i=0;i<4;++i) {
            temp[i]= expandedKey[(bytesGenerated-4)+i];
        }

        if (bytesGenerated%16 == 0) {
            RotateWord(temp);

            for (int i=0; i<4; ++i) {
                temp[i]= SBox[temp[i] / 16][temp[i] % 16];
            }

            temp[0]^=Rcon[bytesGenerated / 16];
        }

        for (int i=0;i<4;++i) {
            expandedKey[bytesGenerated]= expandedKey[bytesGenerated-16] ^ temp[i];
            bytesGenerated++;
        }
    }
}


//Round function for encryption
array<unsigned char, 16> SubBytes(const array<unsigned char, 16>& block) {
    array<unsigned char, 16> encryptedBlock;

    for (int i=0;i<16;++i) {
        unsigned char pixel=block[i];
        unsigned char encryptedPixel= SBox[pixel/16][pixel%16];
        encryptedBlock[i]=encryptedPixel;
    }

    return encryptedBlock;
}

array<unsigned char, 16> ShiftRows(const array<unsigned char, 16>& block) {
    array<unsigned char, 16> shiftedBlock;

    for (int row=0;row<4;++row) {
        for (int col=0;col<4;++col) {
            int shiftedCol =(col+row) % 4;
            shiftedBlock[row*4+col]= block[row*4+shiftedCol];
        }
    }

    return shiftedBlock;
}

unsigned char MultiplyHelper(unsigned char a, unsigned char b){
    unsigned char result= 0;
    while (b!=0) {
        if ((b & 0x01)!=0) {
            result^=a;
        }
        unsigned char highBitSet=(a & 0x80);
        a<<=1;
        if (highBitSet!=0) {
            a^=0x1B;
        }
        b>>=1;
    }
    return result;
}

array<unsigned char, 16> MixColumns(const array<unsigned char, 16>& block){
    array<unsigned char,16> mixedBlock;

    for (int col=0;col<4;++col) {
        for (int row=0;row<4;++row) {
            unsigned char result= 0;
            for (int i=0;i<4;++i) {
                unsigned char multiplier= mixColumnsMatrix[row*4+i];
                unsigned char value= block[i*4+col];
                result ^= MultiplyHelper(multiplier,value);
            }
            mixedBlock[row*4+col]=result;
        }
    }

    return mixedBlock;
}


//Round funtion for decryption
array<unsigned char, 16> InvSubBytes(const array<unsigned char, 16>& block){
    array<unsigned char, 16> decryptedBlock;

    for (int i=0;i<16;++i) {
        unsigned char pixel=block[i];
        unsigned char decryptedPixel= InvSBox[pixel/16][pixel%16];
        decryptedBlock[i]= decryptedPixel;
    }

    return decryptedBlock;
}

array<unsigned char, 16> InvMixColumns(const array<unsigned char, 16>& block){
    array<unsigned char, 16> decryptedBlock;

    for (int col=0;col<4;++col) {
        for (int row=0;row<4;++row) {
            unsigned char result= 0;
            for (int i=0;i<4;++i) {
                unsigned char multiplier=invMixColumnsMatrix[row*4+i];
                unsigned char value=block[i*4+col];
                result ^= MultiplyHelper(multiplier,value);
            }
            decryptedBlock[row*4+col]= result;
        }
    }

    return decryptedBlock;
}

array<unsigned char, 16> InvShiftRows(const array<unsigned char, 16>& block){
    array<unsigned char, 16> decryptedBlock;

    for (int row=0;row<4;++row) {
        for (int col=0;col<4;++col) {
            int shiftedCol= (col-row+4) % 4;
            decryptedBlock[row*4+col] = block[row*4+shiftedCol];
        }
    }

    return decryptedBlock;
}



void displayBlock(const array<unsigned char, 16>& block){
    for (const auto& pixel:block) {
        cout<<hex<<static_cast<int>(pixel)<< " ";
    }
    cout<<endl;
}

int main(){
    const int keySize=16;
    unsigned char key[keySize];
    generateKey(key, keySize);

    cout << "Generated Key: ";
    for (int i=0; i<keySize;++i){
        cout <<hex<<static_cast<int>(key[i]);
    }
    cout<<endl;

    const int expandedKeySize=176; 
    unsigned char expandedKey[expandedKeySize];
    ExpandKey((const unsigned char*)key,expandedKey);

    cout << "Expanded Key: ";
    for (int i=0;i<expandedKeySize;++i) {
        cout<<hex<<static_cast<int>(expandedKey[i]);
    }
    cout<<endl;

    Mat image=imread("img2.jpg", IMREAD_COLOR);
    Mat convertedImage;
    int rows=image.rows - image.rows %4;
    int cols=image.cols - image.cols %4;
    resize(image,image,Size(cols, rows));

    cvtColor(image,convertedImage,COLOR_BGR2GRAY);
    namedWindow("Original Image", WINDOW_NORMAL);
    cv::imshow("Original Image", convertedImage);
    cv::waitKey(100);


    const int blockSize=16;
    vector<array<unsigned char,blockSize>> blocks;

    for (int i=0; i<convertedImage.rows; i+=4) {
        for (int j=0;j<convertedImage.cols;j+=4) {
            array<unsigned char, blockSize> block;

            for (int k=0;k<4;++k) {
                for (int l=0;l<4;++l) {
                    block[k*4+l]= convertedImage.at<unsigned char>(i+k, j+l);
                }
            }

            blocks.push_back(block);
        }
    }

    //AES encrytpion calls
    for (auto& block : blocks){
        array<unsigned char, 16> state=block;

        for (int i=0;i<16;++i) {
            state[i]^=expandedKey[i];
        }

        for (int round=1;round<=9;++round){
            state=SubBytes(state);
            state=ShiftRows(state);
            state=MixColumns(state);

            for (int i=0;i<16;++i) {
                state[i] ^= expandedKey[(round*16) + i];
            }
        }

        state=SubBytes(state);
        state=ShiftRows(state);

        for (int i=0;i<16;++i) {
            state[i]^=expandedKey[160+i];
        }

        block = state;

        //displayBlock(state);

    }


    Mat encryptedImage(convertedImage.size(), CV_8UC1);
    int blockIndex = 0;
    for (int i=0;i<convertedImage.rows;i +=4){
        for (int j=0;j<convertedImage.cols;j+=4){
            const auto& block= blocks[blockIndex];

            for (int k=0;k<4;++k) {
                for (int l=0;l<4;++l) {
                    encryptedImage.at<unsigned char>(i+k,j+l) = block[k*4+l];
                }
            }

            blockIndex++;
        }
    }

    namedWindow("Encrypted Image",WINDOW_NORMAL);
    cv::imshow("Encrypted Image",encryptedImage);
    cv::waitKey(100);



    //AES decryption calls
    for (auto& block : blocks){
        array<unsigned char, 16> state=block;

        for (int i=0;i<16;++i){
            state[i]^=expandedKey[160 + i];
        }

        state=InvShiftRows(state);
        state=InvSubBytes(state);

        for (int round=9;round>=1;--round){
            for (int i=0;i<16;++i) {
                state[i]^=expandedKey[(round * 16) + i];
            }

            state=InvMixColumns(state);
            state=InvShiftRows(state);
            state=InvSubBytes(state);
        }

        for (int i=0;i<16;++i){
            state[i]^=expandedKey[i];
        }

        block=state;
        //displayBlock(state);
    }

    Mat decryptedImage(image.size(),CV_8UC1);
    blockIndex=0;
    for (int i=0;i<convertedImage.rows;i+=4) {
        for (int j=0;j<convertedImage.cols;j+=4) {
            const auto& block=blocks[blockIndex];

            for (int k=0;k<4;++k) {
                for (int l=0;l<4;++l) {
                    decryptedImage.at<unsigned char>(i+k,j+l) = block[k*4+l];
                }
            }

            blockIndex++;
        }
    }

    namedWindow("Decrypted Image",WINDOW_NORMAL);
    cv::imshow("Decrypted Image",decryptedImage);
    cv::waitKey(0);

    return 0;
}