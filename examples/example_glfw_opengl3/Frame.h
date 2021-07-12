#pragma once

#include <vector>

#include "imgui.h"

struct FACE
{
    int f = 0;
    int cx = 0;
    int cy = 0;
    int v[4] = { -1,-1,-1,-1 };
    int e[4] = { -1,-1,-1,-1 };
};

class Frame
{
private:
    FACE* m_Faces = nullptr;
    std::vector<int> m_Indices;
    std::vector<int> m_Edges;
    std::vector<ImVec2> m_Vertexs;

    float m_Max_Step = 6.28318f;
    float m_Sub_Step = 1.0f;

    int m_Count_Vertexs_U = 7;
    int m_Count_Vertexs_V = 5;
    int m_Count_Faces_U;
    int m_Count_Faces_V;
    bool m_Close_U = false;
    bool m_Close_V = false;
    bool m_Show_Edges = false;

public:
    void getVertexIndexs(const int& cx, const int& cy, const int& sx, const int& sy, int* out_v);
    void AddVertex(float x, float y);
    int AddIndice(int vIndice);
    int AddEdge(int vEdge);
    void Compute();
    void draw_face(const FACE& vFace);
    const FACE getLeftFace(const FACE& ref);
    const FACE getRightFace(const FACE& ref);
    const FACE getTopFace(const FACE& ref);
    const FACE getBottomFace(const FACE& ref);
    void draw();
};
