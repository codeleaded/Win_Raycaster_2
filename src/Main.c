//#include "C:/Wichtig/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"

int nMapWidth = 16;				
int nMapHeight = 16;

float fPlayerX = 14.7f;			
float fPlayerY = 5.09f;
float fPlayerA = 0.0f;			
float fFOV = 3.14159f / 1.7f;	
float fDepth = 16.0f;			
float fSpeed = 5.0f;
float Steps = 0.001f;

String map;

typedef struct RayCast {
	Vec2 Inter;
	float a;
} RayCast;

char RaycastDDA(Vec2 s,Vec2 t,Vec2* Intersection) {
	Vec2 vRayStart = s;
	Vec2 vRayDir = Vec2_Norm(Vec2_Sub(t,s));
	Vec2 vRayUnitStepSize = {(float)sqrt(1+(vRayDir.y/vRayDir.x)*(vRayDir.y/vRayDir.x)),
							 (float)sqrt(1+(vRayDir.x/vRayDir.y)*(vRayDir.x/vRayDir.y))};
	
	Vec2 vMapCheck = { (int)vRayStart.x,(int)vRayStart.y };
	Vec2 vRayLength1D = { 0.0f,0.0f };
	Vec2 vStep = { 0.0f,0.0f };
	
	if(vRayDir.x<0) {
		vStep.x = -1;
		vRayLength1D.x = (vRayStart.x-(float)(vMapCheck.x))*vRayUnitStepSize.x;
	}else {
		vStep.x = 1;
		vRayLength1D.x = ((float)(vMapCheck.x+1)-vRayStart.x)*vRayUnitStepSize.x;
	}
	if(vRayDir.y<0) {
		vStep.y = -1;
		vRayLength1D.y = (vRayStart.y-(float)(vMapCheck.y))*vRayUnitStepSize.y;
	}else {
		vStep.y = 1;
		vRayLength1D.y = ((float)(vMapCheck.y+1)-vRayStart.y)*vRayUnitStepSize.y;
	}
	
	char bTileFound = 0;
	float fMaxDistance = fDepth;
	float fDistance = 0.0f;	
	while(!bTileFound&&fDistance<fMaxDistance) {
		if(vRayLength1D.x<vRayLength1D.y) {
			vMapCheck.x += vStep.x;
			fDistance = vRayLength1D.x;
			vRayLength1D.x += vRayUnitStepSize.x;
		}else {
			vMapCheck.y += vStep.y;
			fDistance = vRayLength1D.y;
			vRayLength1D.y += vRayUnitStepSize.y;
		}
		
		if(vMapCheck.x<0.0f || vMapCheck.x>=(float)nMapWidth || vMapCheck.y<0.0f || vMapCheck.y>=(float)nMapHeight || String_Get(&map,(int)(vMapCheck.y * nMapWidth + vMapCheck.x))=='#') {
			bTileFound = 1;
		}
	}
	
	if(bTileFound) {
		Intersection->x = vRayStart.x + vRayDir.x * fDistance;
		Intersection->y = vRayStart.y + vRayDir.y * fDistance;
	}
	return bTileFound;
}

Vector CastAllRays() {
	Vec2 Pos = {fPlayerX,fPlayerY};
	Vector list = Vector_New(sizeof(RayCast));
	for(float i = fPlayerA - fFOV*0.5f;i<fPlayerA + fFOV*0.5f;i+=Steps) {
		Vec2 Target = Vec2_Add(Pos,Vec2_Mul((Vec2){(float)cos(i),(float)sin(i)},(Vec2){fDepth,fDepth}));
		Vec2 Intersection;
		char bHit = RaycastDDA(Pos,Target,&Intersection);
		if(bHit) 
			Vector_Push(&list,(RayCast[]){ Intersection,i });
		else	
			Vector_Push(&list,(RayCast[]){ Target,i });
	}
	return list;
}

//void Render2DView(Graphics2D g, FontMetrics m) {
//	Vec2 MouseCell = new Vec2(Mouse.x/CellSize.x,Mouse.y/CellSize.y);
//	veci2 Cell = new veci2((int)MouseCell.x,(int)MouseCell.y);
//	
//	g.setColor(new Color(0,0,0));
//	g.fillRect(0,0,(int)SCREEN_WIDTH,(int)SCREEN_HEIGHT);
//	
//	for(int y = 0;y<MapSize.y;y++) {
//		for(int x = 0;x<MapSize.x;x++) {
//			int cell = Map[(int) (y*MapSize.x+x)];
//			if(cell==1) {
//				g.setColor(new Color(0,0,255));
//				g.fillRect((int)(x*CellSize.x),(int)(y*CellSize.y),(int)CellSize.x,(int)CellSize.y);
//			}
//			g.setColor(new Color(50,50,50));
//			g.drawRect((int)(x*CellSize.x),(int)(y*CellSize.y),(int)CellSize.x,(int)CellSize.y);
//		}
//	}
//	
//	ArrayList<RayCast> list = CastAllRays(me);
//	for(int i = 0;i<list.size();i++) {
//		g.setColor(new Color(50,50,50));
//		g.drawLine((int)(me.p.x*CellSize.x),(int)(me.p.y*CellSize.y),(int)(list.get(i).v.x*CellSize.x),(int)(list.get(i).v.y*CellSize.y));
//	}
//	
//	int pick = (int)(Cell.y*MapSize.x+Cell.x);
//	if(MOUSESTROKES[MOUSE_LEFT].DOWN && pick>=0 && pick<Map.length) Map[pick] = 1;
//	
//	g.setColor(new Color(255,0,0));
//	g.fillArc((int)(me.p.x*CellSize.x),(int)(me.p.y*CellSize.y),4,4,0,360);
//	g.setColor(new Color(0,255,0));
//	g.fillArc((int)Mouse.x,(int)Mouse.y,4,4,0,360);
//}

void Render3DView() {
	RenderRect(0,0,GetWidth(),GetHeight() / 2,Pixel_RGBA(90,90,255,255));
	RenderRect(0,GetHeight() / 2,GetWidth(),GetHeight() / 2,Pixel_RGBA(90,255,90,255));
	
	Vec2 Pos = {fPlayerX,fPlayerY};
	Vector list = CastAllRays();
	for(int i = 0;i<list.size;i++) {
		RayCast* r = (RayCast*)Vector_Get(&list,i);
		float mg = 1.0f / Vec2_Mag(Vec2_Sub(r->Inter,Pos)) / (float)cosf(fPlayerA - r->a);
		float l = mg * (float)GetHeight();
		float light = F32_Abs(mg);
		light = F32_Clamp(light,0.0f,1.0f);
		
		RenderRect((float)i*Steps/fFOV*(float)GetWidth(),1.0f+(float)GetHeight()*0.5f-l*0.5f,1.0f+(float)GetWidth()*Steps/fFOV,l,Pixel_toRGBA(light,light,light,1.0f));
		
		//float fog = (Vec2_Mag(Vec2_Sub(r->Inter,Pos)) / fDepth);
		//fog = F32_Clamp(fog,0.0f,1.0f);
		//RenderRectAlpha((float)i*Steps/fFOV*(float)GetWidth(),1.0f+(float)GetHeight()*0.5f-l*0.5f,1.0f+(float)GetWidth()*Steps/fFOV,l,Pixel_RGBA(90,90,255,(int)(fog * 255)));
	}
	Vector_Free(&list);
}

void Setup(AlxWindow* w){
    map = String_New();
    String_Append(&map,"#########.......");
    String_Append(&map,"#########.......");
    String_Append(&map,"#...............");
    String_Append(&map,"#.......########");
    String_Append(&map,"#..............#");
    String_Append(&map,"#......##......#");
    String_Append(&map,"#......##......#");
    String_Append(&map,"#..............#");
    String_Append(&map,"###............#");
    String_Append(&map,"##.............#");
    String_Append(&map,"#......####..###");
    String_Append(&map,"#......#.......#");
    String_Append(&map,"#......#.......#");
    String_Append(&map,"#..............#");
    String_Append(&map,"#......#########");
    String_Append(&map,"#..............#");
    String_Append(&map,"################");
}

void Update(AlxWindow* w){
    if (Stroke(ALX_KEY_A).DOWN)
		fPlayerA -= (fSpeed * 0.75f) * w->ElapsedTime;

	if (Stroke(ALX_KEY_D).DOWN)
		fPlayerA += (fSpeed * 0.75f) * w->ElapsedTime;
	
	if (Stroke(ALX_KEY_W).DOWN)
	{
		fPlayerX += cosf(fPlayerA) * fSpeed * w->ElapsedTime;
		fPlayerY += sinf(fPlayerA) * fSpeed * w->ElapsedTime;
		if (String_Get(&map,(int)fPlayerY * nMapWidth + (int)fPlayerX) == '#')
		{
			fPlayerX -= cosf(fPlayerA) * fSpeed * w->ElapsedTime;
			fPlayerY -= sinf(fPlayerA) * fSpeed * w->ElapsedTime;
		}			
	}

	if (Stroke(ALX_KEY_S).DOWN)
	{
		fPlayerX -= cosf(fPlayerA) * fSpeed * w->ElapsedTime;
		fPlayerY -= sinf(fPlayerA) * fSpeed * w->ElapsedTime;
		if (String_Get(&map,(int)fPlayerY * nMapWidth + (int)fPlayerX) == '#')
		{
			fPlayerX += cosf(fPlayerA) * fSpeed * w->ElapsedTime;
			fPlayerY += sinf(fPlayerA) * fSpeed * w->ElapsedTime;
		}
	}

	Render3DView();

	for (int nx = 0; nx < nMapWidth; nx++)
		for (int ny = 0; ny < nMapWidth; ny++)
		{
            if(String_Get(&map,(int)ny * nMapWidth + (int)nx)=='#') RenderRect(nx * 10.0f,ny * 10.0f,10.0f,10.0f,BLACK);
			else                                                	RenderRect(nx * 10.0f,ny * 10.0f,10.0f,10.0f,WHITE);
		}
    RenderRect((fPlayerX) * 10.0f,(fPlayerY) * 10.0f,5.0f,5.0f,GRAY);
}

void Delete(AlxWindow* w){
    String_Free(&map);
}

int main(){
    if(Create("Raycasting World",1900,1000,1,1,Setup,Update,Delete))
        Start();
    return 0;
}