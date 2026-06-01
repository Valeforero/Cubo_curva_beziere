#include "Origin.h"
#include <utility>
#include <cstring>

Origin::Origin() noexcept
{
    // default axes: X (red), Y (green), Z (blue)
    LineVertex axes[] = {
        { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // X start
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // X end
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Y start
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Y end
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Z start
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }  // Z end
    };

    init(axes, sizeof(axes) / sizeof(axes[0]));
}

Origin::Origin(const LineVertex* data, std::size_t count)
{
    init(data, count);
}

Origin::~Origin()
{
    destroy();
}

Origin::Origin(Origin&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_count(other.m_count)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_count = 0;
}

Origin& Origin::operator=(Origin&& other) noexcept
{
    if (this != &other) {
        destroy();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_count = other.m_count;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_count = 0;
    }
    return *this;
}

void Origin::init(const LineVertex* data, std::size_t count)
{
    // destroy existing
    destroy();

    if (count == 0 || data == nullptr)
        return;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    m_count = count;

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_count * sizeof(LineVertex), data, GL_STATIC_DRAW);

    constexpr GLsizei stride = static_cast<GLsizei>(sizeof(LineVertex));
    // position at location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(LineVertex, pos));
    // color at location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(LineVertex, color));

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Origin::update(const LineVertex* data, std::size_t count)
{
    if (data == nullptr || count == 0) {
        return;
    }
    if (m_vbo == 0 || count != m_count) {
        // recreate buffers if size changed or not initialized
        init(data, count);
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(LineVertex), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Origin::draw() const
{
    if (m_vao == 0 || m_count == 0)
        return;

    glBindVertexArray(m_vao);
    // draw lines
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_count));
    glBindVertexArray(0);
}

void Origin::destroy() noexcept
{
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    m_count = 0;
}
