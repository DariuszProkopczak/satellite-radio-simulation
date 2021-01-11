// Source: Uchicago MPCS51045 Advanced C++

#ifndef INDENT_STREAM_H
#  define INDENT_STREAM_H

#include <streambuf>
#include <iostream>
#include <fstream>
#include <memory>
using std::ostream;
using std::streambuf;

using std::ostream;
using std::streambuf;
using std::cout;
using std::cin;
using std::endl;

class IndentStreamBuf : public streambuf
{
public:
    IndentStreamBuf(ostream &stream)
        : wrappedStreambuf(stream.rdbuf()), isLineStart(true), myIndent(0) {}
	virtual int overflow(int outputVal) override
	{
		if (outputVal == traits_type::eof())
			return traits_type::eof();
        if(outputVal == '\n') {
            isLineStart = true;
        } else if(isLineStart) {
            for(size_t i = 0; i < myIndent; i++) {
               wrappedStreambuf->sputc(' ');
            }
            isLineStart = false;
        }
        wrappedStreambuf->sputc(static_cast<char>(outputVal));
		return outputVal;
	}
protected:
    streambuf *wrappedStreambuf;
    bool isLineStart;
public:
    size_t myIndent;
};

class IndentStream : public ostream
{
public:
    IndentStream(ostream &wrappedStream)
      : ostream(new IndentStreamBuf(wrappedStream)) {
    }
    ~IndentStream() { delete this->rdbuf(); }
};

ostream &indent(ostream &ostr)
{
    IndentStreamBuf *out = dynamic_cast<IndentStreamBuf *>(ostr.rdbuf());
	if (nullptr != out) {
        out->myIndent += 4;
    }
    return ostr;
}

ostream &unindent(ostream &ostr)
{
    IndentStreamBuf *out = dynamic_cast<IndentStreamBuf *>(ostr.rdbuf());
    out->myIndent -= 4;
    return ostr;
}

#endif
