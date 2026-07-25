#include <iostream>

// goto allows you to jump to a different place in code to 
// jump from the current line it was called straight to wherever else
// it's been placed.

// just don't use this? then why did learncpp have a lesson on this lmao

void skipping(bool skip) {
    if (skip) {
        goto end;
    }
    std::cout << "Didn't skip lmao\n";
    end: // since the spot we jump to is within the same function,
         // code says we r in the same scope and we're good.
    
}


int main(){

    skipping(true);
    skipping(false);

    return 0;
}