struct tcp_message_header
{
    long version;
    char magic_num[5];
    size_t message_len;
    char reserved;
};
struct tcp_message
{

    // note: this return the pointer to the memory,
    // please use quickly and no need to free the memory
    typedef std::function<void(tcp_message *, int)> on_message_f;

    tcp_message_header header;
    char *buf;

    tcp_message()
    {
        header.version = 1;
        header.magic_num = {'M', 'a', 'g', 'i', 'c'};
        header.message_len = 0;
        header.reserved = 0;
        buf = NULL;
    }
    ~tcp_message()
    {
    }
    bool is_valid()
    {
        string tmp_str(&header.magic_num, 5);
        return (tmp_str == "Magic");
    }

    size_t get_len()
    {
        return (header.message_len + sizeof(tcp_message_header));
    }

    char *get_payload()
    {
        return buf;
    }
    // note, need to provide the message address and free the message after usage.
    tcp_message *form_msg(char *buff, size_t len)
    {
        header.message_len = len;
        header.buf = buf;
        return this;
    }
    // this fucntion form the TCP message
    // note: you need to free the input memory after calling this function.
    // this functin will re-alloc memory, will change it to realloc() later
    tcp_message *append_msg(tcp_message *msg, char *buf, size_t len)
    {
        size_t msg_len = msg->message_len + len + sizeof(tcp_message_header);
        // use realloc later
        char *msg_ptr = malloc(msg_len);
        if (!msg_ptr)
        {
            return NULL;
        }
        if (!memcpy(msg_ptr, msg, msg->message_len + sizeof(tcp_message_header)))
        {
            return NULL;
        }
        if (!memcpy(msg_ptr + len, buf, len))
        {
            return NULL;
        }
        
        return msg_ptr;
    }

    tcp_message *get_message(ring_buffer &buffer)
    {
        tcp_message *_buf = (tcp_message *)(buffer.get_element());
        if (!(_buf->is_valid()))
        {
            return NULL;
        }
    }

    void on_message(tcp_message *)
    {
        (get_message_cb())(tcp_message *);
    }
    void set_message_cb(on_message_f msg_cb) { cb = msg_cb; };
    on_message_f get_message_cb() { return cb; };
  private:
    on_message_f cb;
};
// this fucntion format a new message, delete the memory after use.
tcp_message *form_msg(char *buf, size_t len)
{
    tcp_message msg = new tcp_message();
    tcp_message *ret_ptr = append_msg(msg, buf, len);
    delete msg;
    return ret_ptr;
}
std::mutex message_mutex;
std::condition_variable message_cond;
typedef std::unique_lock<std::mutex> message_lock;
// this function will wait for the signal from translib and then read the ring_buffer
// if the signal do not come, just wait here
void get_message(ring_buffer &buffer)
{
    bool have_next = true;
    int num_left = 0;
    char *left_p = NULL;
    while (1)
    {
        // wait for the signal from translib
        message_lock _lock(message_mutex);
        message_cond.wait(_lock);
        // now we got a ring buffer message
        // 1.1 if there is no message between two ring buffer message element
        tcp_message *_buf = (tcp_message *)(buffer.get_element());
        if (!(_buf->is_valid()))
        {
            continue;
        }
        left_p = get_message(_buff, RB_ELEMENT_SIZE, num_after, have_next);
    }
}
// this function get a message in a ring buffer
char *bool get_message(tcp_message *buff, int num_before, int &num_after, bool &have_next)
{
    while (1)
    {
        if (buff->len <= num_before)
        {
        }
    }
}