#!/usr/local/bin/thrift --gen cpp

namespace cpp example

service Example {
  i32 ping(),
  string echo(1:string s1)
}
