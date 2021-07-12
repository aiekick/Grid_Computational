#pragma once

#include <vector>

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
    std::vector<int> m_Vertexs;
    std::vector<int> m_Edges;

    int m_Count_Vertexs_U = 7;
    int m_Count_Vertexs_V = 5;
    int m_Count_Faces_U;
    int m_Count_Faces_V;
    bool m_Close_U = false;
    bool m_Close_V = false;
    bool m_Show_Edges = false;

public:
    FACE AddFace(const int& faces_index, const int& sx, const int& sy, const int& verts_index, int& edges_index);
    int AddVertex(int vVertex);
    int AddEdge(int vEdge);
    void Compute();
    void draw_face(const FACE& vFace);
    const FACE getLeftFace(const FACE& ref);
    const FACE getRightFace(const FACE& ref);
    const FACE getTopFace(const FACE& ref);
    const FACE getBottomFace(const FACE& ref);
    void draw();
};
