/*
 Copyright (c) 2015, Kotaro Endo.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <node.h>
#include <node_buffer.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace blocking_socket {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::String;
using v8::Object;
using v8::Value;
using v8::Exception;

int openBody(const char* name, size_t size) {
	sockaddr_un addr;
	if (size >= sizeof(addr.sun_path)) {
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	memcpy(addr.sun_path, name, size);
	int fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		return -1;
	}
	int err = connect(fd, (struct sockaddr*) &addr, sizeof(addr));
	if (err) {
		close(fd);
		return -1;
	}
	return fd;
}

int closeBody(int fd) {
	return close(fd);
}

int sendBody(int fd, const char* buf, size_t size) {
	return write(fd, buf, size);
}

int recvBody(int fd, char* buf, size_t size) {
	return read(fd, buf, size);
}

void openMethod(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	String::Utf8Value name(args[0]->ToString());
	int fd = openBody(*name, name.length());
	args.GetReturnValue().Set(Number::New(isolate, fd));
}

void closeMethod(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	int fd = args[0]->ToInteger()->NumberValue();
	int err = closeBody(fd);
	args.GetReturnValue().Set(Number::New(isolate, err));
}

void sendMethod(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	if (args.Length() < 2 || !node::Buffer::HasInstance(args[1])) {
		isolate->ThrowException(
				Exception::TypeError(
						String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int fd = args[0]->ToInteger()->NumberValue();
	int len = sendBody(fd, node::Buffer::Data(args[1]),
			node::Buffer::Length(args[1]));
	args.GetReturnValue().Set(Number::New(isolate, len));
}

void recvMethod(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	if (args.Length() < 2 || !node::Buffer::HasInstance(args[1])) {
		isolate->ThrowException(
				Exception::TypeError(
						String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}
	int fd = args[0]->ToInteger()->NumberValue();
	int len = recvBody(fd, node::Buffer::Data(args[1]),
			node::Buffer::Length(args[1]));
	args.GetReturnValue().Set(Number::New(isolate, len));
}

void init(Local<Object> exports) {
	NODE_SET_METHOD(exports, "open", openMethod);
	NODE_SET_METHOD(exports, "close", closeMethod);
	NODE_SET_METHOD(exports, "send", sendMethod);
	NODE_SET_METHOD(exports, "recv", recvMethod);
}

NODE_MODULE(addon, init)

}
