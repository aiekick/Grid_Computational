#include "Frame.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

void Frame::AddVertex(float x, float y)
{
    m_Vertexs.push_back(ImVec2(x, y));
}

int Frame::AddIndice(int vIndice)
{
    m_Indices.push_back(vIndice);
    return vIndice;
}

int Frame::AddEdge(int vEdge)
{
    m_Edges.push_back(vEdge);
    return vEdge;
}

void Frame::getVertexIndexs(const int& cx, const int& cy, const int& sx, const int& sy, const int& verts_index, int *out_v)
{
    const int lx2 = (sx - 2);
    const int lx1 = (sx - 1);
    const int ly2 = (sy - 2);

    out_v[0] = verts_index;
    out_v[1] = out_v[0] + 1;
    
    if (m_Close_U && m_Close_V)
    {
        if (cx == lx2 && cy == ly2)
        {
            out_v[0] = verts_index + cx;
            out_v[1] = verts_index;
            out_v[2] = 0;
            out_v[3] = lx2;
            return; // fast quit
        }
        else
        {
            out_v[2] = verts_index + sx;
            out_v[3] = verts_index + lx1;

            if (cx == lx2)
            {
                out_v[1] = out_v[0] - lx2;
                out_v[2] = out_v[1] + lx1;
                return; // fast quit
            }
            else if (cy == ly2)
            {
                out_v[0] = verts_index + cx;
                out_v[1] = out_v[0] + 1;
                out_v[3] = out_v[0] - cy * lx1;
                out_v[2] = out_v[3] + 1;
                return; // fast quit
            }
        }
    }
    else
    {
        if (m_Close_V)
        {
            out_v[2] = out_v[1] + sx;
            out_v[3] = out_v[2] - 1;
            if (cy == ly2)
            {
                out_v[0] = verts_index + cx;
                out_v[1] = out_v[0] + 1;
                out_v[3] = out_v[0] - cy * sx;
                out_v[2] = out_v[3] + 1;
            }
            return; // fast quit
        }
        else if (m_Close_U)
        {
            out_v[2] = verts_index + sx;
            out_v[3] = verts_index + lx1;
            if (cx == lx2)
            {
                out_v[1] = out_v[0] - lx2;
                out_v[2] = out_v[1] + lx1;
            }
            return; // fast quit
        }
        else
        {
            out_v[2] = out_v[1] + sx;
            out_v[3] = out_v[2] - 1;
            return; // fast quit
        }
    }
}

void Frame::Compute()
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
    m_Indices.clear();
    m_Edges.clear();

    m_Count_Vertexs_U = ceilf(m_Max_Step / std::max(m_Sub_Step, 0.1f));

    m_Count_Faces_U = m_Count_Vertexs_U - 1;
    m_Count_Faces_V = m_Count_Vertexs_V - 1;

    m_Faces = new FACE[m_Count_Faces_U * m_Count_Faces_V];

    int verts_index = 0;
    int vert_index_for_face = 0; // custom index for comptue poly
    int face_index = 0;

    float step_f = m_Max_Step / (float)(m_Count_Vertexs_U - 1);

    float t = 0.0f;

    for (int v = 0; v < m_Count_Vertexs_V; ++v)
    {
        for (int u = 0; u < m_Count_Vertexs_U; ++u)
        {
            t = 0.0f + u * step_f;

            if (u < m_Count_Faces_U && v < m_Count_Faces_V)
            {
                FACE face;

                getVertexIndexs(u, v, m_Count_Vertexs_U, m_Count_Vertexs_V, vert_index_for_face, face.v);

                face.cx = u;
                face.cy = v;
                face.f = face_index++;

                m_Faces[v * m_Count_Faces_U + u] = face;
            }

            AddVertex(cosf(t) * 200.0f, sin(t) * 200.0f);

            AddIndice(verts_index++);

            if ((m_Close_U && u == (m_Count_Vertexs_U - 2)) ||
                (m_Close_V && v == (m_Count_Vertexs_V - 2)))
                continue;

            vert_index_for_face++;
        }
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

    ImGui::TextColored(getGoodColor(is_V03_Ok), "v%03i", vFace.v[3]); ImGui::SameLine();
    if (m_Show_Edges)
    {
        ImGui::Text("---"); ImGui::SameLine();
        ImGui::TextColored(getGoodColor(is_e02_Ok), "e%03i", vFace.e[2]);
        ImGui::SameLine();
        ImGui::Text("---"); ImGui::SameLine();
    }
    else
    {
        ImGui::Text("-----------"); ImGui::SameLine();
    }
    ImGui::TextColored(getGoodColor(is_V02_Ok), "v%03i", vFace.v[2]);

    if (m_Show_Edges)
        ImGui::Text(" |       |        |");
    else
        ImGui::Text(" |                |");

    if (m_Show_Edges)
    {
        ImGui::TextColored(getGoodColor(is_e03_Ok), "e%03i", vFace.e[3]); ImGui::SameLine();
        ImGui::Text("---"); ImGui::SameLine();
        ImGui::Text("%03i", vFace.f); ImGui::SameLine();
        ImGui::Text("---"); ImGui::SameLine();
        ImGui::TextColored(getGoodColor(is_e01_Ok), "e%03i", vFace.e[1]);
    }
    else
    {
        ImGui::Text(" |                |");
    }
    
    if (m_Show_Edges)
        ImGui::Text(" |       |        |");
    else
        ImGui::Text(" |                |");

    ImGui::TextColored(getGoodColor(is_V00_Ok), "v%03i", vFace.v[0]); ImGui::SameLine();
    if (m_Show_Edges)
    {
        ImGui::Text("---"); ImGui::SameLine();
        ImGui::TextColored(getGoodColor(is_e00_Ok), "e%03i", vFace.e[0]);
        ImGui::SameLine();
        ImGui::Text("---"); ImGui::SameLine();
    }
    else
    {
        ImGui::Text("-----------"); ImGui::SameLine();
    }
    ImGui::TextColored(getGoodColor(is_V01_Ok), "v%03i", vFace.v[1]);

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
    change |= ImGui::Checkbox("Close V", &m_Close_V); ImGui::SameLine();
    change |= ImGui::Checkbox("Show Edges", &m_Show_Edges);
    ImGui::PushItemWidth(200.0f);
    change |= ImGui::InputInt("Count Vertexs U", &m_Count_Vertexs_U); ImGui::SameLine(350.0f);
    change |= ImGui::InputInt("Count Vertexs V", &m_Count_Vertexs_V);
    ImGui::PopItemWidth();
    change |= ImGui::SliderFloat("Max Step", &m_Max_Step, 0.1f, 6.28318f, "%.5f");
    change |= ImGui::SliderFloat("Sub Step", &m_Sub_Step, 0.1f, 6.28318f, "%.5f");
    if (change)
        Compute();

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

    ImGui::Begin("Indices");

    bool is_I_Ok = false;
    for (size_t i = 0; i < m_Indices.size(); i++)
    {
        is_I_Ok = true;
        if (i < m_Indices.size() - 1)
            is_I_Ok = (m_Indices[i] != m_Indices[i + 1]);

        ImGui::TextColored(getGoodColor(is_I_Ok), "v%02i", m_Indices[i]);
    }

    ImGui::End();

    ImGui::Begin("Vertexs");

    bool is_V_Ok = false;
    for (size_t i = 0; i < m_Vertexs.size(); i++)
    {
        ImGui::Text("v%03i : %.2f, %.2f", i, m_Vertexs[i].x, m_Vertexs[i].y);
    }

    ImGui::End();

    if (m_Show_Edges)
    {
        ImGui::Begin("Edges");

        bool is_E_Ok = false;
        for (size_t i = 0; i < m_Edges.size(); i++)
        {
            is_V_Ok = true;
            if (i < m_Edges.size() - 1)
                is_E_Ok = (m_Edges[i] != m_Edges[i + 1]);

            ImGui::TextColored(getGoodColor(is_E_Ok), "e%03i", m_Edges[i]);
        }

        ImGui::End();
    }

    ImGui::Begin("Shape");

    auto win = ImGui::GetCurrentWindowRead();
    auto draw_list = win->DrawList;
    auto win_pos = ImGui::GetWindowPos();
    auto win_size = ImGui::GetWindowSize();
    auto center = win_pos + win_size * 0.5f;

    ImVec2 p0, p1;
    for (size_t i = 0; i < m_Vertexs.size(); i++)
    {
        if (i)
        {
            p0 = m_Vertexs[i-1] + center;
            p1 = m_Vertexs[i] + center;
            draw_list->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(1,1,1,1)), 2.0f);
            draw_list->AddCircleFilled(p1, 5.0f, ImGui::GetColorU32(ImVec4(0, 1, 0, 1)));
        }
    }

    ImGui::End();
}
