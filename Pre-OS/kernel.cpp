extern "C" void main(){
    *(char*)0xb8000 = 'L';
    return;
}