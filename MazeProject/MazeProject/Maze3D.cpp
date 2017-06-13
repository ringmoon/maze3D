#include "Angel.h"

//edge class
class edge {
public:
	vec2 points[2];		//a edge has two points
	void setEdge(vec2 point1, vec2 point2) {
		points[0] = point1;
		points[1] = point2;
	}
	void through() {
		points[0] = points[1] = vec2(0, 0);
	}
	vec2 getPoint1() {
		return points[0];
	}
	vec2 getPoint2() {
		return points[1];
	}
	bool edgeIsExisted() {
		return !(points[0].x == points[1].x&&points[0].y == points[1].y);
	}
};
//grid class
class grid {
public:			//a grid has four edges
	edge up;	//up edae
	edge down;	//down edge
	edge right;	//right edge
	edge left;	//left edge
	bool isVisited;	//whether be visited
	grid() {}
	void setGrid(vec2 a, vec2 b, vec2 c, vec2 d) {
		up.setEdge(a, b);
		right.setEdge(b, c);
		down.setEdge(c, d);
		left.setEdge(d, a);
		isVisited = false;
	}
	//get points of edges
	void getEdgesPoints(vec2 points[], int index) {
		points[index] = up.getPoint1();
		points[index + 1] = up.getPoint2();
		points[index + 2] = right.getPoint1();
		points[index + 3] = right.getPoint2();
		points[index + 4] = down.getPoint1();
		points[index + 5] = down.getPoint2();
		points[index + 6] = left.getPoint1();
		points[index + 7] = left.getPoint2();
	}
};

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int WIDTH = 500;
const int HEIGHT = 500;
const int col_count = 10;
const int row_count = 10;
const int NumVertices = col_count * row_count*4*4;
const float space = 2.5f;   //gird width
int maze_area = col_count * row_count;
float up_down = 0;
float playerLookAtX = 0;
float playerLookAtZ = 0;
float angleNumber = 45;
int Index = 0;
int firstVisitGridIndex = rand() % (col_count*row_count);	//Create a visit start grid
int touchWallCount = 0;

grid grids[col_count*row_count];	//create ROW*COL girds
point4 eye(1, 1.5, 1, 1.0);

GLuint bufferMaze;
GLuint vaoMaze;
GLuint  model_view;
GLfloat  zNear = 0.5, zFar = 100.0;
GLuint  projection;

point4 points[NumVertices];
color4 colors[NumVertices];

color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

//----------------------------------------------------------------------------
//visit method
void visitGrid(int visitingGridIndex, int fromDirection) {
	//case: 0->up 1->right 2->down 3->left 
	// -1->first go in function
	grids[visitingGridIndex].isVisited = true;
	int willGoDirection;
	int nextVisitGridIndex;
	int willVisitDirections[4];
	int visitDirections;

	//find direction of last visit gird
	if (fromDirection == -1) {
		willVisitDirections[3] = rand() % 4;
		visitDirections = 4;
	}
	else {
		willVisitDirections[3] = fromDirection;
		visitDirections = 3;
	}

	//create will visit order to willVisitDirection array
	for (int i = 0; i < 3; i++) {
		int index = rand() % 4;
		willVisitDirections[i] = index;
		if (willVisitDirections[i] == willVisitDirections[3]) {
			i--;
			continue;
		}
		for (int j = 0; j < i; j++) {
			if (willVisitDirections[i] == willVisitDirections[j]) {
				i--;
				break;
			}
		}
	}

	for (int i = 0; i < visitDirections; i++) {
		switch (willVisitDirections[i])
		{
		case 0:
			if (((visitingGridIndex - col_count) >= 0 && !grids[visitingGridIndex - col_count].isVisited)) {
				nextVisitGridIndex = visitingGridIndex - col_count;
				grids[visitingGridIndex].up.through();
				grids[nextVisitGridIndex].down.through();
				willGoDirection = 0;
				visitGrid(nextVisitGridIndex, (willGoDirection + 2) % 4);
			}
			break;
		case 1:
			if ((((visitingGridIndex + 1) % col_count) != 0 && (visitingGridIndex + 1) <row_count*col_count && !grids[visitingGridIndex + 1].isVisited)) {
				nextVisitGridIndex = visitingGridIndex + 1;
				grids[visitingGridIndex].right.through();
				grids[nextVisitGridIndex].left.through();
				willGoDirection = 1;
				visitGrid(nextVisitGridIndex, (willGoDirection + 2) % 4);
			}
			break;
		case 2:
			if (((visitingGridIndex + col_count) < row_count*col_count && !grids[visitingGridIndex + col_count].isVisited)) {
				nextVisitGridIndex = visitingGridIndex + col_count;
				grids[visitingGridIndex].down.through();
				grids[nextVisitGridIndex].up.through();
				willGoDirection = 2;
				visitGrid(nextVisitGridIndex, (willGoDirection + 2) % 4);
			}
			break;
		case 3:
			if ((((visitingGridIndex - 1) % col_count) != (col_count - 1) && (visitingGridIndex - 1) >= 0 && !grids[visitingGridIndex - 1].isVisited)) {
				nextVisitGridIndex = visitingGridIndex - 1;
				grids[visitingGridIndex].left.through();
				grids[nextVisitGridIndex].right.through();
				willGoDirection = 3;
				visitGrid(nextVisitGridIndex, (willGoDirection + 2) % 4);
			}
			break;
		default:
			break;
		}
	}
}


void
quad(point4 vertices[4])
{
	
	colors[Index] = vertex_colors[1]; points[Index] = vertices[0]; Index++;
	colors[Index] = vertex_colors[2]; points[Index] = vertices[1]; Index++;
	colors[Index] = vertex_colors[5]; points[Index] = vertices[2]; Index++;
	colors[Index] = vertex_colors[1]; points[Index] = vertices[0]; Index++;
	colors[Index] = vertex_colors[5]; points[Index] = vertices[2]; Index++;
	colors[Index] = vertex_colors[7]; points[Index] = vertices[3]; Index++;
	// std::cout << Index << std::endl;
}

// OpenGL initialization
point4* getWallPoints(edge e) {
	float maze_radio = 2;
	point4 vertices[4] = {
		point4(e.getPoint2().x, maze_radio*-0.5,e.getPoint2().y,1.0),
		point4(e.getPoint1().x, maze_radio*-0.5,e.getPoint1().y,1.0),
		point4(e.getPoint1().x, maze_radio*0.5,e.getPoint1().y,1.0),
		point4(e.getPoint2().x, maze_radio*0.5,e.getPoint2().y,1.0)
	};
	return vertices;
}

void init()
{
	//找出col_num*row_num個gtids座標
	vec2 startA = vec2((col_count / 2)*(-space), (row_count / 2)*space);
	vec2 startD = vec2((col_count / 2)*(-space), (row_count / 2)*(space)-space);
	for (int i = 0; i < row_count; i++) {
		for (int j = 0; j < col_count; j++) {
			grids[i*col_count + j].setGrid(startA + j*vec2(space, 0), startA + (j + 1)*vec2(space, 0), startD + (j + 1)*vec2(space, 0), j*vec2(space, 0) + startD);
		}
		startA = startD;
		startD += vec2(0, -space);
	}
	//迷宮演算法
	visitGrid(firstVisitGridIndex, -1);

	//clear repeat walls
	for (int i = 0; i < row_count; i++) {
		for (int j = 0; j < col_count; j++) {
			if (i >= 0&&i<(col_count-1)) {
				grids[i*col_count + j].down.through();
			}
			if (j > 0) {
				grids[i*col_count + j].left.through();
			}
		}
	}
	//出口
	grids[0].up.through();
	
	//將每個邊立成牆的前置作業
	for (int i = 0; i < maze_area; i++) {

		if (grids[i].down.edgeIsExisted()) {
			point4* vertices = getWallPoints(grids[i].down);
			quad(vertices);
		}
		if (grids[i].right.edgeIsExisted()) {
			point4* vertices = getWallPoints(grids[i].right);
			quad(vertices);
		}
		if (grids[i].up.edgeIsExisted()) {
			point4* vertices = getWallPoints(grids[i].up);
			quad(vertices);
		}
		if (grids[i].left.edgeIsExisted()) {
			point4* vertices = getWallPoints(grids[i].left);
			quad(vertices);
		}
	}
	glGenVertexArrays(1, &vaoMaze);
	glBindVertexArray(vaoMaze);
	glGenBuffers(1, &bufferMaze);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMaze);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	GLuint program = InitShader("vshader41.glsl", "fshader41.glsl");
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}
//找出相機在哪個Grid內，回傳grids[]索引值
int cameraAtWhichGrid(point4 eye) {
	float minX, minZ, maxX, maxZ;
	point4 startA = point4((col_count / 2)*(-space),1, (row_count /2)*space,1.0);
	for (int i = 0; i < row_count; i++) {
		for (int j = 0; j < col_count; j++) {
			minX = (startA + j*point4(space, 0, 0, 0)).x;
			maxX = (startA + (j + 1)*point4(space, 0, 0, 0)).x;
			minZ= (startA - (i + 1)*point4(0, 0, space, 0)).z;
			maxZ = (startA - i*point4(0, 0, space, 0)).z;
			if (minX <= eye.x && eye.x <= maxX && minZ <= eye.z && eye.z <= maxZ) {
				return i*col_count+j;
			}
		}
	}
	return -1;
}
//----------------------------------------------------------------------------

void
display(void)
{
	double angle = angleNumber * M_PI / 180;
	//移動下一個點事前的宣告
	point4 nextEye(eye.x, eye.y, eye.z, eye.w);
	//為了做到整個 Camera不是完全貼牆
	//所以我們用了nextNextEye去跟牆之間保持點距離 才不會等到 Camera完全貼到牆上才發生碰撞(這樣視線比較好)
	//如果有按下上或下 up_down才會變化 如果是正的就是往前，負的就是往後，記得用完後要設定回0喔
	point4 nextNextEye(eye.x, eye.y, eye.z, eye.w);
	nextEye.x = eye.x + cos(angle) * up_down;
	nextEye.z = eye.z + sin(angle) * up_down;
	nextNextEye.x = eye.x + cos(angle) * up_down * 5;
	nextNextEye.z = eye.z + sin(angle) * up_down * 5;
	up_down = 0;

	//取得目前的座標 跟 未來的座標，所在迷宮的位置
	int cameraCurrentIndex = cameraAtWhichGrid(eye);
	//std::cout << cameraCurrentIndex << std::endl;
	int cameraFutureIndex = cameraAtWhichGrid(nextNextEye);

	 //std::cout << "CAMERA AT: " << cameraCurrentIndex << std::endl;
	 //std::cout << "FUTURE CAMERA AT: " << cameraFutureIndex << std::endl;

	//開始處理碰撞事件
	switch (cameraFutureIndex - cameraCurrentIndex) {
	case 0:
		eye = nextEye;
		break;
	case -1: // 往左走
		if (cameraFutureIndex == -1) {	//如果找到出口(出口設置是grids[0]上方)，就可以直接出去
			eye = nextEye;
			//std::cout <<"從"<< cameraCurrentIndex<< "向上走到"<< cameraFutureIndex << std::endl;
		}
		else if (!grids[cameraFutureIndex].right.edgeIsExisted()) {
			eye = nextEye;
			//std::cout << "從" << cameraCurrentIndex << "向左走到" << cameraFutureIndex << std::endl;
		}
		else {
			std::cout << "撞牆啦!!! X " << ++touchWallCount << std::endl;
		}

		break;
	case -col_count: // 往上走
		if (!grids[cameraCurrentIndex].up.edgeIsExisted()) {
			eye = nextEye;
			//std::cout << "從" << cameraCurrentIndex << "向上走到" << cameraFutureIndex << std::endl;
		}
		else {
			std::cout << "撞牆啦!!! X " << ++touchWallCount<< std::endl;
		}

		break;
	case 1: // 往右走
		if (!grids[cameraCurrentIndex].right.edgeIsExisted()) {
			eye = nextEye;
			//std::cout << "從" << cameraCurrentIndex << "向右走到" << cameraFutureIndex << std::endl;
		}
		else {
			std::cout << "撞牆啦!!! X " << ++touchWallCount << std::endl;
		}

		break;
	case col_count: // 往下走
		if (!grids[cameraFutureIndex].up.edgeIsExisted()) {
			eye = nextEye;
			//std::cout << "從" << cameraCurrentIndex << "向下走到" << cameraFutureIndex << std::endl;
		}
		else {
			std::cout << "撞牆啦!!! X " << ++touchWallCount << std::endl;
		}

		break;
	}


	playerLookAtX = eye.x + cos(angle) * 10;
	playerLookAtZ = eye.z + sin(angle) * 10;

	// std::cout << eye.x << ", " << eye.y << ", " << eye.z << ", " << angleNumber << std::endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	point4  at(playerLookAtX, 0.0, playerLookAtZ, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4  mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	mat4  p = Perspective(45, (float)WIDTH / HEIGHT, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray(bufferMaze);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glEnable(GL_LIGHT0);
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	}

	glutPostRedisplay();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//按下 上、下、左、右 會觸發的事件
void specialInput(int key, int x, int y)
{
	float step = 1.5;
	switch (key)
	{
	case GLUT_KEY_UP:
		up_down = 0.2	;
		break;
	case GLUT_KEY_DOWN:
		up_down = -0.2;
		break;
	case GLUT_KEY_LEFT:
		angleNumber -= 3;
		//do something here
		break;
	case GLUT_KEY_RIGHT:
		angleNumber += 3;
		break;
	}

	glutPostRedisplay();
}
int main(int argc, char **argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1366, 768);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialInput);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
