#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//global variables
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();
int lineNumber = 1;
int lastToken;
int nodeNo = 1;

//Functions for scanner
void addChar();
void getChar();
void getNonBlank();
int lex();

//Functions for parser
void PROGRAM();
void DECL_SEC();
void STMT_SEC();
void DECL();
void ID_LIST();
void ID();
void STMT();
void ASSIGN();
void IFSTMT();
void WHILESTMT();
void INPUT();
void OUTPUT();
void EXPR();
void COMP();
void OPERAND();
void FACTOR();
void INT();
void COMP();
void error();
void printTable();


//character classes
#define LETTER 0
#define DIGIT 1
#define SPECIAL_CHAR 2
#define UNKNOWN 99

//token codes
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define COMMA 27
#define NOT_EQUAL_TO 28
#define COLON 29
#define EOL 30
#define EQUAL_SIGN 31
#define SMALLER_THAN 32
#define BIGGER_THAN 33


/*

    -------------- Creating a tree --------------

*/
struct node {
    int ruleNo;
    int currentBranch;
    int alternative;
    int id_val;
    struct node* first;
    struct node* second;
    struct node* third;
};

struct node* newNode(int rule, int alt, int val) {
    struct node* node = (struct node*)malloc(sizeof(struct node));

    node->ruleNo = rule;
    node->currentBranch = ++nodeNo;
    node->alternative = alt;
    node->id_val = val;

    node->first = NULL;
    node->second = NULL;
    node->third = NULL;

    return (node);
};

//tree to keep track of nodes
struct node* currentNode;
struct node* root;
//struct node* currentNode = root->first;

//main program
int main() {
/* Open the input data file and process its contents */
if ((in_fp = fopen("front.in", "r")) == NULL)
    printf("ERROR - cannot open front.in \n");
 else {
    getChar();
    lex();
    PROGRAM();
    /* do { lex(); } while (nextToken != EOF); */
 }
 return 0;
}

//look up function
int lookup(char ch) {
 switch (ch) {
    case '(':
        //addChar();
        nextToken = LEFT_PAREN;
        break;
    case ')':
        //addChar();
        nextToken = RIGHT_PAREN;
        break;
    case '+':
        //addChar();
        nextToken = ADD_OP;
        break;
    case ',':
        //addChar();
        nextToken = COMMA;
        break;
    case '-':
        //addChar();
        nextToken = SUB_OP;
        break;
    case '*':
        //addChar();
        nextToken = MULT_OP;
        break;
    case '/':
        //addChar();
        nextToken = DIV_OP;
        break;

    case ':':
        //addChar();
        nextToken = COLON;
        break;

    case '<':
        nextToken = SMALLER_THAN;
        break;

    case '>':
        nextToken = BIGGER_THAN;
        break;

    case ';':
        //addChar();
        nextToken = EOL;
        break;
    case '=':
        nextToken = EQUAL_SIGN;
        break;
    default:
        nextToken = EOF;
        break;
 }
 return nextToken;
}

//addChar method
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}

//getChar method
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {

        if (nextChar == '\n') {
            lineNumber++;
        }

        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else if (ispunct(nextChar))
            charClass = SPECIAL_CHAR;
        else charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}

// get non blank
void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}

int lex() {
    lastToken = nextToken;
     lexLen = 0;

     getNonBlank();

     switch (charClass) {
         case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
        nextToken = IDENT;
        break;

    /* Parse integer literals */
     case DIGIT:
        addChar();
        getChar();

        while (charClass == DIGIT) {
            addChar();
            getChar();
        }
        nextToken = INT_LIT;
        break;

    /* checking for assignment operator */
     case SPECIAL_CHAR:
        addChar();
        getChar();

        if (charClass == SPECIAL_CHAR) {
            addChar();
            getChar();
            if (lexeme[0] == ':' && lexeme[1] == '=') {
                nextToken = ASSIGN_OP;
            }
            else if (lexeme[0] == '<' && lexeme[1] == '>') {
                nextToken = NOT_EQUAL_TO;
            }
        } else {
            lookup(lexeme[0]);

        }
        break;

    /* Parentheses and operators */
     /*case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;
    */

    /* EOF */
     case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
     } /* End of switch */

     printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
     return nextToken;
} /* End of function lex */


//writing code for parser

/* PROGRAM -> program DECL_SEC begin STMT_SEC end; */
void PROGRAM() {
    //check if the program starts with program keyword
    if (strcmp("program", lexeme) == 0) {

        printf("\nEntered program\n");

        root = newNode(1,1,0);
        lex();

        currentNode = root->first;
        //going into the decl_section
        DECL_SEC();


        lex();

        //check if there is begin keyword after the decl_section
        if (strcmp("begin",lexeme) == 0) {
            printf("\nEntered begin\n");

                lex();

                currentNode = root->second;
                STMT_SEC();

                if(strcmp("end", lexeme) == 0) {
                        lex();
                        if (nextToken != EOL) {
                            //printf("ERROR: missing ; in line %d", lineNumber);
                            error(";");
                        } else {
                            printf("\nProgram parsed successfully\n");
                            printTable();
                        }
                } else {
                    printf("\nERROR: Program should end with end keyword.\n");
                    exit(0);
                }

        } else {
            //printf("\nMissing begin in line %d",lineNumber);
            error("begin");
        }

    } else {
        printf("\nERROR: Program should begin with program keyword.\n");

        exit(0);
    }

}

/* Rule 02: DECL_SEC -> DECL | DECL DECL_SEC */
void DECL_SEC() {
    printf("\nEntered Decl_sec\n");

    currentNode = newNode(2,1,0);
    currentNode = currentNode->first;
    DECL();
}

/* Rule 03: DECL -> ID_LIST : int ; */
void DECL() {
    printf("\nEntered Decl\n");

    currentNode = newNode(3,1,0);
    currentNode = currentNode->first;
    ID_LIST();

    if (nextToken == COLON) {
        lex();
        if(strcmp("int", lexeme) == 0) {
            lex();
            if (nextToken == EOL) {
              return;
            } else {
                //printf("\nmissing ; on line %d.\n", lineNumber);
                error(";");
            }
        } else error("identifier");
    } else {
        //printf("\nMissing : on line %d.\n", lineNumber);
        error(":");
    }
}
/* ID_LIST -> ID | ID , ID_LIST */
void ID_LIST() {
    printf("\nEntered ID_LIST\n");
    struct node* id_list_node = newNode(4,1,0);

    currentNode = id_list_node;
    currentNode = id_list_node->first;

    ID();

    lex();

    currentNode = id_list_node->second;

    while(nextToken == COMMA) {
        struct node* tempNode = newNode(4,1,0);
        lex();
        currentNode = newNode(4,1,0);
        currentNode = tempNode->first;
        ID();
        lex();

        if (nextToken == COMMA) {
            currentNode = tempNode->second;
        }
    }
}

void ID() {
    if (nextToken == IDENT) {
        printf("\nEntered ID\n");
        currentNode = newNode(5,1,0);
    } else {
        //printf("\nexpected int in line %d.\n", lineNumber);
        error("identifier");
    }
}

void STMT_SEC() {
    printf("entered STMT_SEC\n");

    STMT();
}

// this function tells if the next token is an assignment op
int isStatement() {
    if (strcmp("if", lexeme) == 0 || strcmp("while", lexeme) == 0 || strcmp("input", lexeme) == 0 || strcmp("output", lexeme) == 0 || nextToken == IDENT) {
        return 1;
    }
    return 0;
}

void STMT() {
    printf("entered STMT()\n");

    while(isStatement() == 1) {
        if (strcmp("if", lexeme) == 0) {
            IFSTMT();
        } else if (strcmp("while", lexeme) == 0) {
            WHILESTMT();
        } else if (strcmp("input", lexeme) == 0) {
            INPUT();
        } else if (strcmp("output", lexeme) == 0) {
            OUTPUT();
        } else if (strcmp("end", lexeme) == 0) {
            return;
        } else if (strcmp("else", lexeme) == 0) {
            return;
        }
        else {
            ASSIGN();
        }

        lex();
    }

}

void ASSIGN() {
    printf("entered ASSIGN()\n");
    ID();
    lex();

    if (nextToken == ASSIGN_OP) {
        lex();
        EXPR();

        if (nextToken != EOL) {
            //printf("ERROR: expected ; in line %d\n", lineNumber);
            error(";");
        }
    } else {
        //printf("ERROR: expected := in line %d\n", lineNumber);
        error(":=");
    }
}

void EXPR() {
    printf("entered EXPR\n");

    FACTOR();


    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        FACTOR();
    }
}

void FACTOR() {
    printf("Entered FACTOR\n");

    OPERAND();

    while (nextToken == MULT_OP  ) {
            lex();
            OPERAND();
        }
    printf("Exit FACTOR\n");
    }


void OPERAND() {
    printf("Entered OPERAND()\n");
    if (nextToken == INT_LIT) {
        INT();
        lex();
    } else if (nextToken == IDENT) {
        ID();
        lex();
    } else if (nextToken == LEFT_PAREN) {
        lex();
        EXPR();

        if (nextToken != RIGHT_PAREN) {
            //printf("ERROR: expected ) in line %d", lineNumber);
            error(")");

        }
    }

}

void INT() {
    if (nextToken == INT_LIT) {
        printf("Entered INT\n");
    } else {
        //printf("ERROR: integer in line %d", lineNumber);
        error("integer literal");
    }
}

void IFSTMT() {
    printf("Entered IFSTMT\n");

    lex();

    COMP();

    lex();

    if (strcmp("then", lexeme) == 0) {
        lex();
        STMT_SEC();
        if (strcmp("else", lexeme) == 0) {
            lex();
            STMT_SEC();
        }

        if (strcmp("end", lexeme) == 0) {

            lex();

            if (strcmp("if", lexeme) == 0) {
                    lex();
                if (nextToken != EOL) {
                    //printf("ERROR: expected ; in line %d\n", lineNumber);
                    error(";");
                }
            } else {
                //printf("ERROR: missing in line %d\n", lineNumber);
                error("end if");
            }

        } else {
            //printf("ERROR: missing end if in line %d\n", lineNumber);
            error("end if");
        }
    } else {
        //printf("ERROR: expected then in line %d\n", lineNumber);
        error("then");
    }
}

void WHILESTMT() {
    printf("Entered WHILESTMT()\n");
    lex();
    COMP();

    lex();

    if (strcmp("loop", lexeme) == 0) {
        lex();
        STMT_SEC();

        if (strcmp(lexeme, "end") == 0) {
            lex();
            if (strcmp(lexeme, "loop") == 0) {
                lex();

                if (nextToken != EOL) {
                    //printf("ERROR: missing ; in line %d\n",lineNumber);
                    error(";");
                }
            } else {
                //printf("ERROR: missing end loop in line %d\n",lineNumber);
                error("end loop");
            }
        } else {
            //printf("ERROR: missing end loop in line %d\n",lineNumber);
            error("end loop");
        }
    }

}

void INPUT() {
    printf("Entered INPUT()\n");

    lex();

    ID_LIST();



    if (nextToken != EOL) {
        printf("ERROR: missing ; in line %d\n",lineNumber);
    }
}

void OUTPUT() {
    printf("Entered OUTPUT()\n");

    lex();

    ID_LIST();

    if (nextToken != EOL) {
        printf("ERROR: missing ; in line %d\n",lineNumber);
    }
}

void COMP() {
    printf("Entered COMP()\n");

    if (nextToken == LEFT_PAREN) {
        lex();
        OPERAND();

        if (nextToken == EQUAL_SIGN || nextToken == NOT_EQUAL_TO || nextToken == BIGGER_THAN || nextToken == SMALLER_THAN) {
            lex();
            OPERAND();
        } else {
            //printf("ERROR: missing logical operator in line %d\n",lineNumber);
            error("logical operator");
        }
    } else {
         //printf("ERROR: missing ( in line %d\n",lineNumber);
         error("(");
    }
}

void error(char expected[]) {
    printf("ERROR: expected %s in line %d\n",expected,lineNumber);
    exit(0);
}

void printTable(struct node* root) {
    if (root == NULL) {
        return;
    }
    int firstBranch = root->first == NULL ? 0 : root->first->currentBranch;
    int secondBranch = root->second == NULL ? 0 : root->second->currentBranch;

    printf("%d %d %d %d\n", root->currentBranch, root->ruleNo, firstBranch, secondBranch);
    printTable(root->first);
    printTable(root->second);
}
