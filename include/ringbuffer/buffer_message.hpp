#include "ringbuffer.hpp"
#define MAX_MSG_LEN 10 * 1024
struct tcp_message_header
{
    long version;
    char magic_num[5];
    size_t message_len;
    char reserved;
};
struct tcp_message
{
    tcp_message_header header;
    char *buf;

    tcp_message()
    {
        header.version = 1;
        memcpy(header.magic_num, "Magic", 5); // = {'M', 'a', 'g', 'i', 'c'};
        header.message_len = 0;
        header.reserved = 0;
        buf = NULL;
    }
    tcp_message(char *msg, size_t length)
    {
        header.version = 1;
        memcpy(header.magic_num, "Magic", 5); // = {'M', 'a', 'g', 'i', 'c'};
        header.message_len = length;
        header.reserved = 0;
        buf = msg;
    }
    ~tcp_message()
    {
    }
    // this function check if the message is valid
    bool is_valid()
    {
        string tmp_str(header.magic_num, 5);
        return (tmp_str == "Magic");
    }
    // this function return the whole message length

    size_t get_len()
    {
        //cout << "message header length is : " << sizeof(tcp_message_header) << endl;
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
        buf = buff;
        return this;
    }

    bool get_message(ring_buffer &buffer, char *out_msg)
    {
        cout << "in function get_message()" << endl;

        tcp_message_header tmp_header;
        (buffer.peek(sizeof(tcp_message_header), (char *)(&tmp_header)));
        tcp_message_header *header_p = &tmp_header;

        cout << "get message header, pointer is : " << (void *)header_p << endl;
        if (!header_p)
        {
            return false;
        }

        if (!(((tcp_message *)header_p)->is_valid()))
        {
            cout << "get the message in the ring buffer fail, valid check fail" << endl;
            return false;
        }

        int message_len = 0;
        message_len = (header_p->message_len) + sizeof(tcp_message_header);

        cout << "int get_message() : get message with length :" << message_len << endl;
        //length = message_len;
        if (buffer.get(message_len, out_msg))
        {
            cout << "get message success" << endl;
            //on_message((tcp_message *)out_msg, message_len);
        }
        else
        {
            cout << "get message fail" << endl;
            return false;
        }
        return true;
    }

    bool raw_msg(char *buff, size_t buff_len = 0)
    {
        if (buff_len == 0)
        {
            // do not care buffer length
        }
        else if (((this->header).message_len + sizeof(tcp_message_header)) > buff_len)
        {
            return false;
        }
        memcpy(buff, &(this->header), sizeof(tcp_message_header));
        memcpy(buff + sizeof(tcp_message_header), this->buf, (this->header).message_len);
        return true;
    }
};

bool form_raw_tcp_message(char *buffer, size_t buff_len)
{
    /*
    tcp_message_header header;
    if (buff_len == 0)
    {
        // do not care buffer length
    }
    else if (((this->header).message_len + sizeof(tcp_message_header)) > buff_len)
    {
        return false;
    }
    memcpy(buff, &(this->header), sizeof(tcp_message_header));
    memcpy(buff + sizeof(tcp_message_header), this->buf, (this->header).message_len);
    */
    return true;
}