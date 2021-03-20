#include <cassert>

class Ringbuffer
{
public:
    Ringbuffer(int newsize) :
        m_size(newsize),
        m_size_mask(newsize-1),
        m_size_x2(m_size << 1),
        m_size_x2_mask((m_size << 1) - 1)
    {
        assert(is_power_of_2(m_size));
    }

    int read_space() const
    {
        return (w - r + m_size_x2) & m_size_x2_mask;
    }

    int write_space() const
    {
        return m_size - ((w - r + m_size_x2) & m_size_x2_mask);
    }

    void inc_write_pos(int amnt) { assert(amnt <= write_space()); w = (amnt+w) & m_size_x2_mask; }
    void inc_read_pos(int amnt) { assert(amnt <= read_space()); r = (amnt+r) & m_size_x2_mask; }

    int read_pos() const { return r & m_size_mask; }
    int write_pos() const { return w & m_size_mask; }

    int size() const { return m_size; }

private:

    //! return if x = 2^n for n in [0,...]
    static bool is_power_of_2(int x)
    {
        return x && !(x & (x-1));
    }

    int r = 0; //!< read position
    int w = 0; //!< write position

    const int m_size;         //!< size
    const int m_size_mask;    //!< size - 1
    const int m_size_x2;      //!< size * 2
    const int m_size_x2_mask; //!< size * 2 - 1
};
