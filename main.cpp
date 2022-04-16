#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define PI 3.14

void init()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    gluOrtho2D(0, 500, 0, 500);
}

/* Data Structure for each node in the tree */
struct node
{
    int key;
    struct node *left;
    struct node *right;
    int height;
};

// Global Variables - Start
struct node* root = NULL;           /* Initialize Root Node to NULL */
struct node* current=NULL;          /* Initialize Current Node for highlighting(GreenColor)*/

int result=0,page=0;                       /* Store the current number to be inputted*/

/* Coordinates of each rectangle (for reference)
    int startRect[][2] = {
        {-12,-7},
        { 9,-7},
        { 9, -2},
        {-12, -2}
    };

    int aboutRect[][2] = {
        {-12,-13},
        { 9,-13},
        { 9, -8},
        {-12, -8}
    };

    int creditRect[][2] = {
        {-12,-19},
        { 9,-19},
        { 9, -14},
        {-12, -14}
    };

    int exitRect[][2] = {
        {-12,-25},
        { 9,-25},
        { 9, -20},
        {-12, -20}
    };
*/

int rectangles[4][4][2] = {
    {
        {-12,-7},
        { 9,-7},
        { 9, -2},
        {-12, -2}
    },
    {
        {-12,-13},
        { 9,-13},
        { 9, -8},
        {-12, -8}
    },
    {
        {-12,-19},
        { 9,-19},
        { 9, -14},
        {-12, -14}
    },
    {
        {-12,-25},
        { 9,-25},
        { 9, -20},
        {-12, -20}
    }
};

int rectanglesNav[2][4][2] = {
    {
        {-42, 14},
        {-34, 14},
        {-34, 17},
        {-42, 17}
    },
    {
        { 38, 14},
        { 46, 14},
        { 46, 17},
        { 38, 17}
    }
};

int currentBox = -1, backExit=-1;

// Global Variables - End

// Function Prototypes
void display(void);
void drawFirstPage();
void printCredits();
void printAbout();

/* A utility function to get height of the tree*/
int height(struct node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

/* Function that allocates a new node with the given key. */
struct node* newNode(int key)
{
    struct node* node = (struct node*)
                        malloc(sizeof(struct node));
    node->key   = key;
    node->left   = NULL;
    node->right  = NULL;
    node->height = 1;  /* New node is initially added at leaf*/
    current=node;
    return(node);
}

/* A utility function to right rotate subtree rooted with y*/
struct node *rightRotate(struct node *y)
{
    struct node *x = y->left;
    struct node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right))+1;
    x->height = max(height(x->left), height(x->right))+1;

    // Return new root
    return x;
}

/* A utility function to left rotate subtree rooted with x */
struct node *leftRotate(struct node *x)
{
    struct node *y = x->right;
    struct node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = max(height(x->left), height(x->right))+1;
    y->height = max(height(y->left), height(y->right))+1;

    // Return new root
    return y;
}

/* Function to Get Balance factor of node N */
int getBalance(struct node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds*10 + clock();
    while (goal > clock());
}

/* Function to Insert a node into the tree and perform balancing*/
struct node* insert(struct node* node, int key)
{
    /* 1.  Perform the normal BST rotation */
    if (node == NULL)
        return(newNode(key));

    sleep(1);

    if (key < node->key)
        node->left  = insert(node->left, key);
    else if(key > node->key)
        node->right = insert(node->right, key);

    display();      /* Display Tree after normal BST Insertion */

    sleep(1);

    /* 2. Update height of this ancestor node */
    node->height = max(height(node->left), height(node->right)) + 1;


    /* 3. Get the balance factor of this ancestor node to check whether
       this node became unbalanced */
    int bal = getBalance(node);

    /* Left Left Rotation case */
    if (bal > 1 && key <= node->left->key)
         return rightRotate(node);

    /* Right Right Rotation case */
    if (bal < -1 && key >= node->right->key)
        return leftRotate(node);

    /* Left Right Rotation case */
    if (bal > 1 && key >= node->left->key)
    {
        node->left =  leftRotate(node->left);
        display();
        sleep(1);
        return rightRotate(node);
    }

   /* Right Left Rotation case */
    if (bal < -1 && key <= node->right->key)
    {
        node->right = rightRotate(node->right);

        display();
        sleep(1);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

/* Function to draw a line to connect nodes */
void draw_line(float x1,float y1,float x2, float y2)
{
    glBegin(GL_LINES);
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glEnd();
}



/* Function to display the text */
void draw_text(char* text,float x, float y)
{
    int i;
    glRasterPos3f(x-0.4, y-0.4, 1);
    for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
	int i;
	int triangleAmount = 20; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * PI;

	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(i = 0; i <= triangleAmount;i++) {
			glVertex2f(
		            x + (radius * cos(i *  twicePi / triangleAmount)),
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius){
	int i;
	int lineAmount = 100; //# of triangles used to draw circle

	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * PI;

	glBegin(GL_LINE_LOOP);
		for(i = 0; i <= lineAmount;i++) {
			glVertex2f(
			    x + (radius * cos(i *  twicePi / lineAmount)),
			    y + (radius* sin(i * twicePi / lineAmount))
			);
		}
	glEnd();
}

/* A recursive function to draw a binary tree */
void drawNode(struct node* t_root,float x1,float y1,int level)
{
    char buff[5];

    if (t_root==NULL)
    return;

    float radius = 1.5;
    float left_angle = 250;
    float right_angle = 110;
    float branch_length = 21 - level*2.5;
    float angle_change = 20;

    /* Draw the current node */
    if(t_root==current)
        glColor3f(0.0,1.0,0.0);           /* If the node to be drawn is the currently inputted node
                                Set color of node to green until its placed in its position */
    else
        glColor3f(1.0,0.0,0.0);     /* else set color of node to red */



    drawFilledCircle(x1, y1, radius);
    glColor3f(0, 0, 0);
    drawHollowCircle(x1, y1, radius);

    sprintf(buff,"%d",t_root->key); //atoi
    glColor3f(1.0,1.0,1.0);
    draw_text(buff,x1,y1);
            /* Display value of the node*/

    if(t_root->left)    /*Checks if parent has a left child node to be drawn*/
    {
        /* Draw the left child node */
        float angle = left_angle - level*angle_change;
        double radian = angle*3.14/180;
        //float m = (double)tan((double)radian);
        float x2 = x1 + branch_length * sin((double) radian);
        float y2 = y1 + branch_length * cos((double) radian);
        drawNode(t_root->left,x2,y2,level+1);
        glColor3f(0.0,0.0,0.0);
        draw_line(x1,y1,x2,y2);
    }
    if(t_root->right) /*Checks if parent has a Right child node to be drawn*/
    {
        /* Draw the Right child node */
        float angle = right_angle + level*angle_change;
        float radian = angle*3.14/180;
        //float m = (double)tan((double)radian);
        float x2 = x1 + branch_length * sin((double) radian);
        float y2 = y1 + branch_length * cos((double) radian);
        drawNode(t_root->right,x2,y2,level+1);

        glColor3f(0.0,0.0,0.0);
        draw_line(x1,y1,x2,y2);
    }
}

void drawBox(int rectangle[][2])
{
    glBegin(GL_LINE_LOOP);
        for(int i=0; i<4; i++)
            glVertex2iv(rectangle[i]);
    glEnd();
}

void drawBoxes()
{
    for(int i=0; i<4; i++)
    {
        if(i==currentBox)
            glColor3f(1, 0, 0);
        else
            glColor3f(0.8, 0.8, 0.8);

        drawBox(rectangles[i]);
    }
}

void displayNav()
{
    glColor3f(0.0,0.0,0.0);

    glRasterPos3f(-40, 15, 0);
    char text[50] = " BACK ";
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

    strcpy(text, " AVL TREE ");
    glRasterPos3f(-6, 14, 0);
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);

    glRasterPos3f( 40, 15, 0);
    strcpy(text, " EXIT ");
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);



    glColor3f(1, 0, 0);
    if(backExit != -1)
        drawBox(rectanglesNav[backExit]);

    glColor3f(0.8, 0.8, 0.8);
    drawBox(rectanglesNav[1]);
    drawBox(rectanglesNav[0]);


    glColor3f(1, 0, 0);

    if(backExit != -1)
        rectanglesNav[backExit];
}

void display()
{

    if(page==0)
    {
        glClearColor (1,1,1,1.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0,10,-30);
        glColor3f(1, 0, 0);

        drawBoxes();
        drawFirstPage();
        glutSwapBuffers();
    }
    if(page==1)
    {
        glClearColor (1.5,1.0,1.0,1.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0,10,-30);
        glColor3f(1,1,1);

        drawNode(root,0, 12, 0);
        displayNav();

        glutSwapBuffers();
    }
    if(page==2)
    {
        glClearColor (1,1,1,1.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0,10,-30);
        glColor3f(1,1,1);

        printAbout();
        displayNav();
        glutSwapBuffers();
    }

    if(page==3)
    {
        glClearColor (1,1,1,1.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0,10,-30);
        glColor3f(1,1,1);

        printCredits();
        displayNav();
        glutSwapBuffers();
    }
    if(page==4)
        exit(0);
}
void printAbout()
{
    int i;
    char buffer[50]="What is an AVL tree?. ";
    glColor3f(0.0,0.0,0.0);
    glRasterPos3f(-11,-0.5,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);

     strcpy(buffer,"1.AVL tree is a self-balancing binary search tree");
    glRasterPos3f(-11,-5.5,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);


    strcpy(buffer,"2. The heights of the two child subtrees of ");
    glRasterPos3f(-11,-9,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);

    strcpy(buffer," any node differ by at most one ");
    glRasterPos3f(-11,-11,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);

    strcpy(buffer,"3. If at any time they differ by more than one,");
    glRasterPos3f(-11,-15,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);

    strcpy(buffer,"     rebalancing is done to the tree ");
    glRasterPos3f(-11,-17,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);


}

void printCredits()
{
    int i;
    char buffer[50]="Project By ";
    glColor3f(0.0,0.0,0.0);
    glRasterPos3f(-5,-0.5,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);

    strcpy(buffer,"Dhanush Raj");
    glRasterPos3f(-5,-5.5,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);


    strcpy(buffer,"Dinesh Kudva");
    glRasterPos3f(-5,-9,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buffer[i]);
}
void drawFirstPage()
{
    int i;
    char buffer[50]="AVL TREE CONSTRUCTION ";
    glColor3f(0.0,0.0,0.0);

    glRasterPos3f(-12,1,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);

    strcpy(buffer,"1. START");
    glRasterPos3f(-5,-5.5,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);

    strcpy(buffer,"2. ABOUT");
    glRasterPos3f(-5,-11,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);

    strcpy(buffer,"3. CREDITS");
    glRasterPos3f(-5,-16.5,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);

    strcpy(buffer,"4. EXIT");
    glRasterPos3f(-5,-22,1.5);
    for (i = 0; buffer[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (90, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
    glMatrixMode (GL_MODELVIEW);
    // printf("%d, %d", w, h);
}

void keyboard(unsigned char key,int x,int y)
{
    if(page==0)
    {
        switch(key)
        {
            case '1':
                page=1;
                display();
                break;

            case '2':
                page=2;
                display();
                break;

            case '3':
                page=3;
                display();
                break;

            case '4':
                exit(0);
        }
    }
    else if(page==1)
    {
        switch(key)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                result=result*10+(key-'0');
                break;
            case 'z':
                root=insert(root,result);
//               sleep(0.5);
                display();
                result=0;
                break;
            case 'x':
                exit(0);
            }
    }
    else if(page==2)
    {
            page=0;
            display();
    }
    else if(page==3)
    {
            page=0;
            display();
    }
}

void checkMousePosition(int x, int y)
{

}

void mouse(int btn, int state, int x, int y)
{
    if(page == 0)
    {
        if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
        {
            page = currentBox+1;
            display();
        }
    }
    else
    {
        if(y>45 && y<85)
        {
            if(x <= 770)
            {
                page = 0;
                display();
            }
            else
            {
                exit(0);
            }
        }
    }
}

void mousePassiveMotion(int x, int y)
{
    if(page==0)
    {
        if(y>290 && y<600)
        {
            if(y>290 && y<=364)
            {
                currentBox = 0;
            }
            else if(y>364 && y<=442)
            {
                currentBox = 1;
            }
            else if(y>442 && y<525)
            {
                currentBox = 2;
            }
            else
            {
                currentBox = 3;
            }
        }
        else
        {
            currentBox = -1;
        }

        display();
    }
    else
    {
        if(y>45 && y<85)
        {
            if(x <= 770)
            {
                backExit = 0;
            }
            else
            {
                backExit = 1;
            }
        }
        else
        {
            backExit = -1;
        }
        display();
    }
}

int main (int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowPosition (0, 0);
    glutInitWindowSize (840, 640);
    glutCreateWindow ("AVL tree : A self balancing B-Tree");
    //init();
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutKeyboardFunc (keyboard);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(mousePassiveMotion);

    glEnable(GL_DEPTH_TEST);

    glutMainLoop ();
    return 0;
}
