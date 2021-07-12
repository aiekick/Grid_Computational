#include "Frame.h"
#include "imgui.h"
#include "imgui_internal.h"

int Frame::AddVertex(int vVertex)
{
    m_Vertexs.push_back(vVertex);
    return vVertex;
}

int Frame::AddEdge(int vEdge)
{
    m_Edges.push_back(vEdge);
    return vEdge;
}

/*void Frame::AddFace(int cx, int cy)
{

}
*/

void Frame::init()
{
    /* poly p
      v03 -e02- v02
       |         |
      e03   p   e01
       |         |
      v00 -e00- v01
    */

    if (m_Faces)
        delete[] m_Faces;

    m_Vertexs.clear();
    m_Edges.clear();

    m_Count_Faces_U = m_Count_Vertexs_U - 1;
    m_Count_Faces_V = m_Count_Vertexs_V - 1;

    m_Faces = new FACE[m_Count_Faces_U * m_Count_Faces_V];

    int verts_index = 0;
    int edge_index = 0;
    int face_index = 0;
    int last_row_new_edges = 0;
    for (int v = 0; v < m_Count_Vertexs_V; ++v)
    {
        int last_new_edges = 0;
        int row_new_edges = 0;
        for (int u = 0; u < m_Count_Vertexs_U; ++u)
        {
            if (u < m_Count_Faces_U && v < m_Count_Faces_V)
            {
                FACE face;

                /////////////////////////////////////////////////////
                /////////////////////////////////////////////////////
                /////////////////////////////////////////////////////

                int new_edges = 0;
                face.v[0] = verts_index;
                face.v[1] = face.v[0] + 1;
                face.v[2] = face.v[1] + m_Count_Faces_U;
                face.v[3] = face.v[2] - 1;
                if (v == 0)
                {
                    face.e[0] = AddEdge(edge_index++); new_edges++;
                }
                else
                {
                    if (u == 0)
                    {
                        face.e[0] = edge_index - last_row_new_edges + 2;
                    }
                    else
                    {
                        face.e[0] = edge_index - last_row_new_edges + 3;
                    }
                }
                face.e[1] = AddEdge(edge_index++); new_edges++;
                face.e[2] = AddEdge(edge_index++); new_edges++;
                if (u == 0)
                {
                    face.e[3] = AddEdge(edge_index++); new_edges++;
                }
                else
                {
                    face.e[3] = edge_index - 2 - last_new_edges;
                }

                last_new_edges = new_edges;
                row_new_edges += new_edges;

                /////////////////////////////////////////////////////
                /////////////////////////////////////////////////////
                /////////////////////////////////////////////////////

                face.cx = u;
                face.cy = v;
                face.f = face_index++;// edge_index;

                m_Faces[v * m_Count_Faces_U + u] = face;

                AddVertex(verts_index++);
            }
        }
        last_row_new_edges = row_new_edges;
    }
}

const FACE Frame::getLeftFace(const FACE& ref)
{
    if (ref.cx > 0)
        return m_Faces[ref.cy * m_Count_Faces_U + (ref.cx - 1)];
    if (m_Close_U)
        return m_Faces[ref.cy * m_Count_Faces_U + (ref.cx + m_Count_Faces_U - 1)];
    return FACE();
}

const FACE Frame::getRightFace(const FACE& ref)
{
    if (ref.cx < (m_Count_Faces_U - 1))
        return m_Faces[ref.cy * m_Count_Faces_U + (ref.cx + 1)];
    if (m_Close_U)
        return m_Faces[ref.cy * m_Count_Faces_U + (0)];
    return FACE();
}

const FACE Frame::getTopFace(const FACE& ref)
{
    if (ref.cy < (m_Count_Faces_V - 1))
        return m_Faces[(ref.cy + 1) * m_Count_Faces_U + ref.cx];
    if (m_Close_V)
        return m_Faces[(0) * m_Count_Faces_U + ref.cx];
    return FACE();
}

const FACE Frame::getBottomFace(const FACE& ref)
{
    if (ref.cy > 0)
        return m_Faces[(ref.cy - 1) * m_Count_Faces_U + ref.cx];
    if (m_Close_V)
       return m_Faces[(ref.cy + m_Count_Faces_V - 1) * m_Count_Faces_U + ref.cx];
    return FACE();
}

const bool cmpFaces(const int& ref, const int& other)
{
    if (other == -1)
        return true;
    return ref == other;
}

static const ImVec4 getGoodColor(bool flag)
{
    return (flag ? ImVec4(0.2f, 1.0f, 0.2f, 1.0f) : ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
}

void Frame::draw_face(const FACE& vFace)
{
/* poly p
  v03 -e02- v02
   |         |
  e03   p   e01
   |         |
  v00 -e00- v01
*/

    const bool is_V00_Ok = (cmpFaces(vFace.v[0], getLeftFace(vFace).v[1]) && cmpFaces(vFace.v[0], getBottomFace(vFace).v[3]));
    const bool is_V01_Ok = (cmpFaces(vFace.v[1], getRightFace(vFace).v[0]) && cmpFaces(vFace.v[1], getBottomFace(vFace).v[2]));
    const bool is_V02_Ok = (cmpFaces(vFace.v[2], getRightFace(vFace).v[3]) && cmpFaces(vFace.v[2], getTopFace(vFace).v[1]));
    const bool is_V03_Ok = (cmpFaces(vFace.v[3], getLeftFace(vFace).v[2]) && cmpFaces(vFace.v[3], getTopFace(vFace).v[0]));

    const bool is_e00_Ok = (cmpFaces(vFace.e[0], getBottomFace(vFace).e[2]));
    const bool is_e01_Ok = (cmpFaces(vFace.e[1], getRightFace(vFace).e[3]));
    const bool is_e02_Ok = (cmpFaces(vFace.e[2], getTopFace(vFace).e[0]));
    const bool is_e03_Ok = (cmpFaces(vFace.e[3], getLeftFace(vFace).e[1]));

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::PushItemWidth(30.0f);

    ImGui::TextColored(getGoodColor(is_V03_Ok), "v%02i", vFace.v[3]); ImGui::SameLine();
    ImGui::Text(" - "); ImGui::SameLine();
    ImGui::TextColored(getGoodColor(is_e02_Ok), "e%02i", vFace.e[2]); ImGui::SameLine();
    ImGui::Text(" - "); ImGui::SameLine();
    ImGui::TextColored(getGoodColor(is_V02_Ok), "v%02i", vFace.v[2]);

    ImGui::Text(" |"); ImGui::SameLine();
    ImGui::Text("     "); ImGui::SameLine();
    ImGui::Text("|"); ImGui::SameLine();
    ImGui::Text("     "); ImGui::SameLine();
    ImGui::Text("|");

    ImGui::TextColored(getGoodColor(is_e03_Ok), "e%02i", vFace.e[3]); ImGui::SameLine();
    ImGui::Text(" - "); ImGui::SameLine();
    ImGui::Text("%03i", vFace.f); ImGui::SameLine();
    ImGui::Text(" - "); ImGui::SameLine();
    ImGui::TextColored(getGoodColor(is_e01_Ok), "e%02i", vFace.e[1]);

    ImGui::Text(" |"); ImGui::SameLine();
    ImGui::Text("     "); ImGui::SameLine();
    ImGui::Text("|"); ImGui::SameLine();
    ImGui::Text("     "); ImGui::SameLine();
    ImGui::Text("|");

    ImGui::TextColored(getGoodColor(is_V00_Ok), "v%02i", vFace.v[0]); ImGui::SameLine();
    ImGui::Text(" - "); ImGui::SameLine();
    ImGui::TextColored(getGoodColor(is_e00_Ok), "e%02i", vFace.e[0]); ImGui::SameLine();
    ImGui::Text(" - "); ImGui::SameLine();
    ImGui::TextColored(getGoodColor(is_V01_Ok), "v%02i", vFace.v[1]);

    ImGui::PopItemWidth();

    ImGui::EndGroup();
}

void Frame::draw()
{
    ImGui::Begin("Grid Computation");

    ImGui::Separator();

    bool change = false;
    change |= ImGui::Button("Refresh"); ImGui::SameLine();
    change |= ImGui::Checkbox("Close U", &m_Close_U); ImGui::SameLine();
    change |= ImGui::Checkbox("Close V", &m_Close_V);
    ImGui::PushItemWidth(200.0f);
    change |= ImGui::InputInt("Count Vertexs U", &m_Count_Vertexs_U);
    change |= ImGui::InputInt("Count Vertexs V", &m_Count_Vertexs_V);
    ImGui::PopItemWidth();
    if (change)
        init();

    ImGui::Text("Count Vertexs U | V : %i | %i", m_Count_Vertexs_U, m_Count_Vertexs_V);
    ImGui::Text("Count Faces U | V : %i | %i", m_Count_Faces_U, m_Count_Faces_V);

    for (int v = (m_Count_Faces_V - 1); v >=0; --v)
    {
        ImGui::Separator();

        for (int u = 0; u < m_Count_Faces_U; ++u)
        {
            if (u)
                ImGui::SameLine();

            draw_face(m_Faces[v * m_Count_Faces_U + u]);
        }

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    }

    ImGui::Separator();

    ImGui::End();

    ImGui::Begin("Vertexs");

    bool is_V_Ok = false;
    for (size_t i = 0; i < m_Vertexs.size(); i++)
    {
        is_V_Ok = true;
        if (i < m_Vertexs.size() - 1)
            is_V_Ok = (m_Vertexs[i] != m_Vertexs[i + 1]);

        ImGui::TextColored(getGoodColor(is_V_Ok), "v%02i", m_Vertexs[i]);
    }

    ImGui::End();

    ImGui::Begin("Edges");

    bool is_E_Ok = false;
    for (size_t i = 0; i < m_Edges.size(); i++)
    {
        is_V_Ok = true;
        if (i < m_Edges.size() - 1)
            is_E_Ok = (m_Edges[i] != m_Edges[i + 1]);

        ImGui::TextColored(getGoodColor(is_E_Ok), "e%02i", m_Edges[i]);
    }

    ImGui::End();
}
