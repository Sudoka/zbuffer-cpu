#pragma once

#include <vector>
#include <list>
#include <map>
#include "BasicStructure.h"
#include "Camera.h"
#include "RenderState.h"

class QImage;
class CPoint3D;

class CScanLine
{
private:

	// 3D vertex
	class Vertex 
	{
	public:
		Vec4d posWorld;			// position in world space
		Vec3d posScreen;		// position in screen space
		Color4d color;			// color
		Normald normalWorld;	// normal in world space
		Vertex(double _x, double _y, double _z, double _w)
			: posWorld(_x, _y, _z, _w)
		{
		}
	};

	// ����yMax���߷�����Ӧ������
	class Edge
	{
	public:
		double x;			// �ߵ��϶˵��x����
		double dx;			// ��������ɨ���߽����x�����
		int dy;				// �߿�Խ��ɨ������Ŀ���൱��yMax

		Color4d color;		// �˵���ɫ
		Color4d dclr;		// ����ɨ������ɫ��

		Normald normalW;	// �˵㷨��World Space��
		Normald dNorW;	// ����ɨ���߷���World Space��

		Vec4d posW;			// �˵�λ�ã�World Space��
		Vec4d dPosW;		// ����ɨ����λ�òWorld Space��

		int id;				// ����������α��
	};

	class ActiveEdge 
	{
	public:
		Edge *el, *er;	// ���ұ�
						/*	�Ժ��󽻵����ڱ��ཻ��ɨ������Ϊ��ֵ��
							�Ժ�����ÿ����һ��ɨ���߼�1 */		

		double zl;		/*	�󽻵㴦���������ƽ������ֵ��*/
		double dzx;		/*	��ɨ���������߹�һ������ʱ�����������
							ƽ����������������ƽ�淽�̣�dzx��-a/c
							(c��0)��*/
		double dzy;		/*	��y���������ƹ�һ��ɨ����ʱ���������
							��ƽ����������������ƽ�淽�̣�dzy��b/c
							(c��0)��*/
	//	int id;		// �Ѿ����Դ�Edge�еõ���
	};

	// ����yMax������η�����Ӧ������
	class Triangle
	{
	public:
		Vec3d normal;	// �淨��(Screen Space��
		double d;		// surface equation
		int dy;			// ����ο�Խ��ɨ������Ŀ
	};

	typedef std::list<Edge*> EdgeList;
	typedef EdgeList::iterator EListIterator;

	typedef std::map<int, EdgeList, std::less<int> > EdgeTable;
	typedef EdgeTable::iterator ETableIterator;

	typedef std::vector<Triangle*> TriangleArray;
	typedef TriangleArray::iterator TArrayItor;

	typedef std::map<int, ActiveEdge, std::less<int> > ActiveEdgeList;
	typedef ActiveEdgeList::iterator AEListItor;

	typedef std::vector<Vertex*> VertexBuffer;
	typedef VertexBuffer::iterator VBufferItor;

	//typedef std::vector<Color4u> ColorBuffer;
	//typedef std::vector<Normald> NormalBuffer;
	//typedef NormalBuffer::iterator NorBufItor;

	//typedef std::vector<Light> LightArray;
	//typedef LightArray::iterator LightItor;

	typedef std::vector<int> IndexBuffer;
	typedef IndexBuffer::iterator IBufferItor;

public:
	CScanLine();
	CScanLine(int _w, int _h, QImage *_img);
	~CScanLine(void);

	void setRenderTarget(int _w, int _h, QImage *_img);

	// start create target
	void begin(TargetType _type);
	void end();

	void vertex3d(double _x, double _y, double _z);
	void vertex3dv(const Vec3d& v);
	void vertex3fv(const CPoint3D& v);
	void normal3d(double _nx, double _ny, double _nz);
	void normal3fv(const CPoint3D& n);
	void color3f(float _r, float _g, float _b);
	void color4f(float _r, float _g, float _b, float _a);
	void color3i(unsigned char _r, unsigned char _g, unsigned char _b);
	void color4i(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a);

	//void setLight(const Light& light);
	void clear(int _target, const Color4u& _c = Color4u(0,0,0,255), double _depth = 1.0);

	// camera related, facade design model
	void lookAt(const Vec3d& eye, const Vec3d& at, const Vec3d& up);
	void perspective(double fovy, double aspect, double zNear, double zFar);
	void frustum(double left, double right, double bottom, double top, double near, double far);
	void ortho(double left, double right, double bottom, double top, double near, double far);

	void setRenderState(int _state, int _val);
	const CRenderState& renderState() { return mRenderState; }

private:
	void _init();
	void _clear();
	bool _compare_edges(const Edge* e1, const Edge* e2);
	bool _addEdge(const Vertex* _v1, const Vertex* _v2, int _id);
	void _addATriangle(const Vertex *_v1, const Vertex *_v2, const Vertex *_v3);
	void _addTriangles();
	void _addTriangleStrip();
	void _addTriangleFan();
	void _addQuads();
	void _addQuadStrip();
	void _addPolygon();
	// scan a line at mCurY
	void _scanLine();

	// �����ȼ���
	void _calculateLight(const Vec4d& _pos, const Normald& _nor, Color4d& _clr);
	void _setFrameBuffer(int _y, int _x, Color4d& _clr);

	void _modelViewProjectionTransform();
	void _normalizeDeviceCoordinates();
	void _screenCoordinates();

	// Step 1. Vertex Transformation
	void _vertexTransform();

private:
	bool mbInitialised;		// �Ƿ����ú�RenderTarget
	bool mbHasNormals;		// �Ƿ��з���������
	int mMaxY;				// contained
	int mCurY;				// contained
	TargetType mType;
	int mHeight, mWidth;
	QImage *mImg;

	EdgeTable mSortedET;		// sorted edge table
	ActiveEdgeList mAEL;		// active edge list
	TriangleArray mTriArray;	// sorted polygon table

	VertexBuffer mVertexBuffer; // ����
	IndexBuffer mIndexBuffer;	// ����
	std::vector<double> mZBuffer;

	//ColorBuffer mColorBuffer;	
	//NormalBuffer mNormalBuffer;	

	Color4d mCurColor;		// ��ɫ
	Normald mCurNormal;		// ����

	CCamera mCamera;		// �����

	CRenderState mRenderState;		// ����״̬����
	//LightArray mLights;

public:
	Material mMaterial;		// ��������
	Light mLight;
	Color4d mGlobalAmbient; // ȫ�ֻ�����
};
