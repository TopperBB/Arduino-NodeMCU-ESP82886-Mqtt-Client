
int StringLen(const char* source){
  int len = 0;
  while (source[len] != '\0' && len < 100) { len++; }
  return len;
}
