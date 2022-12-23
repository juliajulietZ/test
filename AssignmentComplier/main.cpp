#include <iostream>
#include <map>
#include <stack>
#include <cmath>
using namespace std;

//关键字
const int MAX_KEY_NUMBER = 20;
const string KEY_WORD_END = "waiting for your expanding"; /*关键字结束标记*/
string KeyWordTable[MAX_KEY_NUMBER] = { "if","then", "else","while","begin","end", KEY_WORD_END };

//数字
const int DIGIT = 1;
const int POINT = 2;
const int OTHER = 3; //fixme
const int POWER = 4;
const int PLUS = 5;
const int MINUS = 6;
const int EndState = -1;
int w, n, p, e, d;//w:尾数累加器  p:指数累加器  n:十进制小数位数计数器  e:指数的符号  d:数字0-9
int ICON;
double FCON;
int flag;
static int CurrentState;


map<int,string>class_value={
        {7,"ID"},{8,"INT"},{9,"REAL"},{10,"LT"},{11,"LE"},
        {12,"EQ"},{13,"NE"},{14,"GT"},{15,"GE"},{16,"IS"},
        {17,"PL"},{18,"MI"},{19,"MU"},{20,"DI"},{21,"LP"},
        {22,"RP"},{23,"#"}
};

//Lex part
struct Tuple{
    string Class;
    string token;
};
Tuple tupl;
Tuple scanner(FILE*);
int lookup(string);
int EXCUTE(int,int);
int TXQ = 0;
Tuple *pTuple = new Tuple[100];

int rownum = 1;

bool flagLex = true;
Tuple scanner(FILE *fp){
    string TOKEN;
    char ch;
    ch = fgetc(fp);
    if(ch == EOF){
        flagLex = false;
        tupl.Class="#";
        tupl.token=" ";
        return tupl;
    }
    while(' ' == ch || '\n' == ch || '\t' == ch){
        if('\n' == ch) rownum++;
        ch = fgetc(fp);
    }
    if(isalpha(ch)){
        TOKEN += ch;
        ch = fgetc(fp);
        while(isalnum(ch)){
            TOKEN += ch;
            ch = fgetc(fp);
        }
        if((ch != ' ')&&(ch != '\n') && (ch !='\t') &&(ch!= EOF))//todo
        {
            fseek(fp,-1,1);
        }
        if (ch == '\n') rownum++;
        int index = lookup(TOKEN);//查关键字表
        if(-1 == index){//是ID
            tupl.Class=class_value[7];
            tupl.token=TOKEN;
            return tupl;
        }
        else{
            tupl.Class=class_value[index];
            tupl.token=TOKEN;
            return tupl;
        }
    }
    else if(isdigit(ch)){
        fseek(fp, -1, 1);  /* retract*/
        //todo
        CurrentState = 0;
        while(CurrentState != EndState){
            ch = fgetc(fp);
            int symbol = -1;
            if(isdigit(ch)) {d = ch - '0', symbol = DIGIT;}
            if('.' == ch) symbol = POINT;
            if('E' == ch || 'e' == ch) symbol = POWER;
            if (ch == '+') symbol = PLUS;
            if (ch == '-') symbol = MINUS;
            EXCUTE(CurrentState, symbol);
        }
        if((ch != '\n')&&(ch != '\t')&&(ch != ' ')&&(ch != EOF)){
            fseek(fp,-1,1); /* retract*/
        }
        if(ch == '\n') rownum++;
        if(flag == 0){
            tupl.Class=class_value[8];
            tupl.token= to_string(ICON);
            return tupl;
        }
        else if(flag == 1){
            tupl.Class=class_value[9];
            tupl.token= to_string(FCON);
            return tupl;
        }else{
            tupl.Class = "error";
            tupl.token =TOKEN;
            return tupl;
        }

    }
    else{
        switch(ch){
            case '<':{
                ch = fgetc(fp);
                if('=' == ch){
                    tupl.Class=class_value[11];
                    tupl.token="<=";
                    return tupl;
                }
                else {
                    if ((ch != ' ') && (ch != '\n') && (ch != '\t') && (ch != EOF))
                        fseek(fp, -1, 1);
                    if (ch == '\n') rownum++;
                    tupl.Class = class_value[10];
                    tupl.token = "<";
//                    pTuple[TXQ]=tupl;TXQ++;
                    return tupl;
                }
            }
            case '=':{
                tupl.Class = class_value[12]; tupl.token = "=";
//                pTuple[TXQ]=tupl;TXQ++;
                return tupl;
            }
            case '>':{
                ch = fgetc(fp);
                if('=' == ch){
                    tupl.Class=class_value[15];
                    tupl.token=">=";
//                    pTuple[TXQ]=tupl;TXQ++;
                    return tupl;
                }
                else {
                    if ((ch != ' ') && (ch != '\n') && (ch != '\t') && (ch != EOF))
                        fseek(fp, -1, 1);
                    if (ch == '\n') rownum++;
                    tupl.Class = class_value[14];
                    tupl.token = ">";
//                    pTuple[TXQ]=tupl;TXQ++;
                    return tupl;
                }
            }
            case ':':{
                ch = fgetc(fp);
                if('=' == ch){
                    tupl.Class=class_value[16];
                    tupl.token=":=";
//                    pTuple[TXQ]=tupl;TXQ++;
                    return tupl;
                }
                else {
                    if ((ch != ' ') && (ch != '\n') && (ch != '\t') && (ch != EOF))
                        fseek(fp, -1, 1);
                    if (ch == '\n') rownum++;
                    tupl.Class = "error";
                    tupl.token = TOKEN;
//                    pTuple[TXQ]=tupl;TXQ++;
                    return tupl;
                }
            }
            case '+': {tupl.Class = class_value[17]; tupl.token = "+";  return tupl; }//fixme 这部分冗余的写入pTuple[TXQ]全删了
            case '-': {tupl.Class = class_value[18]; tupl.token = "-";  return tupl; }
            case '*': {tupl.Class = class_value[19]; tupl.token = "*";  return tupl; }
            case '/': {tupl.Class = class_value[20]; tupl.token = "/";  return tupl; }
            case '(': {tupl.Class = class_value[21]; tupl.token = "(";  return tupl; }
            case ')': {tupl.Class = class_value[22]; tupl.token = ")";  return tupl; }
            case ';': {tupl.Class = class_value[23]; tupl.token = ";";  return tupl; }//#号
            default: {tupl.Class = "error"; tupl.token = ""; return tupl; }
        }
    }

}

int EXCUTE (int state, int symbol){
    switch(state){
        case 0:switch (symbol)
            {
                case DIGIT:n = 0; p = 0; e = 1; w = d; CurrentState = 1; break;
                case POINT:w = 0; n = 0; p = 0; e = 1; CurrentState = 3; break;
                default:CurrentState = EndState;
            }break;
        case 1:switch (symbol)
            {
                case DIGIT:w = w * 10 + d; break;//CurrentState=1
                case POINT:CurrentState = 2;
                    break;
                case POWER:CurrentState = 4; break;
                default:ICON = w; flag = 0; CurrentState = EndState;
            }break;
        case 2:switch (symbol)
            {
                case DIGIT:n++; w = w * 10 + d;
                    break;
                case POWER:CurrentState = 4;
                    break;
                default:FCON = w * pow(10, e*p - n); flag = 1; CurrentState = EndState;
            }break;
        case 3:switch (symbol)
            {
                case DIGIT:n++; w = w * 10 + d; CurrentState = 2; break;
                default:flag = -1; CurrentState = EndState;
            }break;
        case 4:switch (symbol)
            {
                case DIGIT:p = p * 10 + d; CurrentState = 6; break;
                case MINUS:e = -1; CurrentState = 5;
                    break;
                case PLUS:CurrentState = 5; break;
                default:flag = -1; CurrentState = EndState;
            }break;
        case 5:switch (symbol)
            {
                case DIGIT:p = p * 10 + d; break;
                default:FCON = w * pow(10, e*p - n);  flag = 1; CurrentState = EndState;
            }break;
        case 6:switch (symbol)
            {
                case DIGIT:p = p * 10 + d; break;
                default:FCON = w * pow(10, e*p - n);  flag = 1; CurrentState = EndState;
            }break;
    }
    return CurrentState;
}

int lookup(string token){//查关键字表
    int n=0;
    while( KeyWordTable[n] != KEY_WORD_END){
        if(KeyWordTable[n] == token){
            return n;//是关键字
        }
        n++;
    }
    return -1;//是标识符ID
}

//Parser part
int SLRtable[20][14]={
        {101,6,102,101,101,101,101,7,105,1,2,3,4,5},
        {101,103,102,101,101,101,101,103,100,-1,-1,-1,-1,-1},
        {8,103,102,101,101,101,101,103,102,-1,-1,-1,-1,-1},
        {101,103,102,9,10,101,101,103,102,-1,-1,-1,-1,-1},
        {101,103,54,54,54,11,12,103,54,-1,-1,-1,-1,-1},
        {101,103,57,57,57,57,57,103,57,-1,-1,-1,-1,-1},
        {101,6,102,101,101,101,101,7,102,-1,-1,13,4,5},
        {60,103,59,59,59,59,59,103,59,-1,-1,-1,-1,-1},
        {101,6,102,101,101,101,101,7,102,-1,-1,14,4,5},
        {101,6,102,101,101,101,101,7,102,-1,-1,-1,15,5},
        {101,6,102,101,101,101,101,7,102,-1,-1,-1,16,5},
        {101,6,102,101,101,101,101,7,102,-1,-1,-1,-1,17},
        {101,6,102,101,101,101,101,7,102,-1,-1,-1,-1,18},
        {104,104,19,9,10,104,104,104,104,-1,-1,-1,-1,-1},
        {101,103,102,9,10,101,101,103,51,-1,-1,-1,-1,-1},
        {101,103,52,52,52,11,12,103,52,-1,-1,-1,-1,-1},
        {101,103,53,53,53,11,12,103,53,-1,-1,-1,-1,-1},
        {101,103,55,55,55,55,55,103,55,-1,-1,-1,-1,-1},
        {101,103,56,56,56,56,56,103,56,-1,-1,-1,-1,-1},
        {101,103,58,58,58,58,58,103,58,-1,-1,-1,-1,-1}
};
//int SLRtable[20][14]={
//        {-1,6,-1,-1,-1,-1,-1,7,-1,1,2,3,4,5},
//        {-1,-1,-1,-1,-1,-1,-1,-1,100,-1,-1,-1,-1,-1},
//        {8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
//        {-1,-1,-1,9,10,-1,-1,-1,-1,-1,-1,-1,-1,-1},
//        {-1,-1,54,54,54,11,12,-1,54,-1,-1,-1,-1,-1},
//        {-1,-1,57,57,57,57,57,-1,-1,-1,-1,-1,-1,-1},
//        {-1,6,-1,-1,-1,-1,-1,7,-1,-1,-1,13,4,5},
//        {60,-1,59,59,59,59,59,-1,59,-1,-1,-1,-1,-1},
//        {-1,6,-1,-1,-1,-1,-1,7,-1,-1,-1,14,4,5},
//        {101,6,103,102,102,102,102,7,102,-1,-1,-1,15,5},
//        {101,6,103,102,102,102,102,7,102,-1,-1,-1,16,5},
//        {101,6,103,102,102,102,102,7,102,-1,-1,-1,-1,17},
//        {101,6,103,102,102,102,102,7,102,-1,-1,-1,-1,18},
//        {101,104,19,9,10,102,102,104,102,-1,-1,-1,-1,-1},
//        {101,104,103,9,10,102,102,104,51,-1,-1,-1,-1,-1},
//        {101,104,52,52,52,11,12,104,52,-1,-1,-1,-1,-1},
//        {101,104,53,53,53,11,12,104,53,-1,-1,-1,-1,-1},
//        {101,104,55,55,55,55,55,104,55,-1,-1,-1,-1,-1},
//        {101,104,56,56,56,56,56,104,56,-1,-1,-1,-1,-1},
//        {101,104,58,58,58,58,58,104,58,-1,-1,-1,-1,-1}
//};
struct Producer{
    string Lstr;
    string Rstr;
};
Producer Pset[11]={{"S","A"},
                   {"A","V=E"},
                   {"E","E+T"},
                   {"E","E-T"},
                   {"E","T"},
                   {"T","T*F"},
                   {"T","T/F"},
                   {"T","F"},
                   {"F","(E)"},
                   {"F","i"},
                   {"V","i"}

};

int EMQ = 0;
struct ErrorMessage{
    int rownum;
    string error;
};
ErrorMessage* errorMessage = new ErrorMessage[100];
void errorLog(int rownum, string error){
    errorMessage[EMQ].rownum = rownum;
    errorMessage[EMQ].error = error;
    EMQ++;
}

//Sema part
int NXQ = 0;
int NXTemp = 1;
struct Quadruple{
    char op;//todo
    string arg1;
    string arg2;
    string result;
};
Quadruple* pQuadr = new Quadruple[100];
string NewTemp(){
    string TempID = "T"+ to_string(NXTemp);
    NXTemp++;
    return TempID;
}
void GEN(char op,string arg1,string arg2,string result){
    pQuadr[NXQ].op = op;
    pQuadr[NXQ].arg1 = arg1;
    pQuadr[NXQ].arg2 = arg2;
    pQuadr[NXQ].result = result;
    NXQ++;
}


bool flagParser = true;
bool mark = true;
bool retract = false;
bool out = false;
string resolution;

int main(){
    FILE *fp1 = fopen("D:\\code\\2022CompilationPrincipleLab\\AssignmentComplier\\text.txt","r");
    while(flagLex){
            pTuple[TXQ] = scanner(fp1);
            TXQ++;
    }
    for(int i = 0; i < TXQ-2; i++) {
        cout << "(" << pTuple[i].Class << "," <<pTuple[i].token << ")" << endl;
    }
    fclose(fp1);
    rownum = 1;

//    fp = fopen("D:\\code\\2022CompilationPrincipleLab\\AssignmentComplier\\text.txt","r");
    FILE *fp = fopen("D:\\code\\2022CompilationPrincipleLab\\AssignmentComplier\\text.txt","r");
    stack<int>state;
    stack<string>symbol;
//    stack<string>scanned;//被扫描的词
    stack<string>var;//用于生成中间语言的变量
    string word;
    state.push(0);
    symbol.push("#");
    Tuple T;
    int act;
    while(!out){
        if(!retract){
            T = scanner(fp);
        }
        word = T.Class;
        if(word == "EQ"){act = SLRtable[state.top()][0];}
        else if(word == "LP"){act = SLRtable[state.top()][1];}
        else if(word == "RP"){act = SLRtable[state.top()][2];}
        else if(word == "PL"){act = SLRtable[state.top()][3];}
        else if(word == "MI"){act = SLRtable[state.top()][4];}
        else if(word == "MU"){act = SLRtable[state.top()][5];}
        else if(word == "DI"){act = SLRtable[state.top()][6];}
        else if(word == "ID"){act = SLRtable[state.top()][7];}
        else if(word == "INT"){act = SLRtable[state.top()][7];}
        else if(word == "REAL"){act = SLRtable[state.top()][7];}
        else if(word == "#"){act = SLRtable[state.top()][8];}
        else{ break;}
        if(act >=1 && act <= 50){
            cout<<"移进"<<T.token<<"("<<word<<")"<<endl;
            if(word == "ID" || word == "INT" ||word == "REAL")
            {
                var.push(T.token);
            }
            symbol.push(word);
            state.push(act);
            if(retract) retract = false;
        }
        else if(act >=50 && act <100){
            if(!feof(fp)&& !retract){//扫描指针回溯1个
                fseek(fp,-1,1);
            }
            cout<<"按产生式"<<Pset[act-50].Lstr<<"->"<<Pset[act-50].Rstr<<"归约"<<endl;
            //调用Sema
            try{
                if (Pset[act - 50].Rstr[1] == '+' ||
                    Pset[act - 50].Rstr[1] == '-' ||
                    Pset[act - 50].Rstr[1] == '*' ||
                    Pset[act - 50].Rstr[1] == '/'||
                    act == 60)
                {
                    if(act == 60)
                    {
                        resolution = var.top();
                    }
                    else{
                        string Arg2 = var.top();
                        var.pop();
                        string Arg1 = var.top();
                        var.pop();
                        string temp = NewTemp();
                        GEN(Pset[act-50].Rstr[1],Arg1,Arg2,temp);
                        var.push(temp);
                    }
                }
                else{
                    if (word == "ID" || word == "INT" || word == "REAL") { var.push(T.token); }
                }
            }
            catch(exception e){
            }
            //parser的状态栈、符号栈出栈
             for(int i=0;i<Pset[act-50].Rstr.length();i++)
             {
                            symbol.pop();
                            state.pop();
             }
            symbol.push(Pset[act-50].Lstr);
//            if(symbol.top() == "A"){state.push(SLRtable[state.top()][9]);}
            if(symbol.top() == "A"){state.push(SLRtable[0][9]);}
            else if(symbol.top() == "V"){state.push(SLRtable[state.top()][10]);}
            else if(symbol.top() == "E"){state.push(SLRtable[state.top()][11]);}
            else if(symbol.top() == "T"){state.push(SLRtable[state.top()][12]);}
            else if(symbol.top() == "F"){state.push(SLRtable[state.top()][13]);}
            else {
                cout<<"表达式归约出错"<<endl;
            }
        }
        else if(100 == act){
            if(mark){
                cout << "分析成功!" << endl;
                while (!state.empty()) { state.pop(); }
                while (!symbol.empty()) { symbol.pop(); }
                while (!var.empty()) { var.pop(); }
                mark = true;
                state.push(0);
                symbol.push("#");
            }
            else{
                cout << "分析失败!" << endl;
                flagParser= false;
                while (!state.empty()) { state.pop(); }
                while (!symbol.empty()) { symbol.pop(); }
                while (!var.empty()) { var.pop(); }
                mark = true;
                state.push(0);
                symbol.push("#");
            }
        }
        else if(105 == act){
            break;
        }
        else{
            mark = false;
            switch (act) {
                case 101: {
                    errorLog(rownum, "缺少运算项");
                    if (!feof(fp)) {
                        fseek(fp, -1, 1);
                    }
                    symbol.push("ID");
                    state.push(7);
                    var.push("miss term");
                    break;
                }
                case 102: {
                    errorLog(rownum, "无法找到与)匹配的左括号");
                    break;
                }
                case 103: {
                    errorLog(rownum, "缺少运算符");
                    if (!feof(fp)) {
                        fseek(fp, 0 - T.token.length(), 1);
                    }
                    T.Class = "PL";
                    T.token = "+";
                    retract = true;
                    break;
                }
                case 104: {
                    errorLog(rownum, "缺少右括号");
                    symbol.push("RP");
                    state.push(15);
                    break;
                }
            }
                
            }

        }

    if(flagParser){
        for (int i = 0; i < NXQ; i++) {
            cout << "(" << pQuadr[i].op << "," + pQuadr[i].arg1 + "," + pQuadr[i].arg2 + "," + pQuadr[i].result + ")" << endl;
        }
        cout << "(" << "=" << "," <<pQuadr[NXQ-1].result<< "," <<"-" << ","<<resolution<< ")" << endl;
    }

    for (int i = 0; i < EMQ; i++) {
        cout << "第" << errorMessage[i].rownum << "行：" << errorMessage[i].error << endl;
    }

    fclose(fp);
    return 0;
}
