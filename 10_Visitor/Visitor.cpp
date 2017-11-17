


class DocStats {
    unsigned int chars_, nonBlankChars_, words_, images_;
    // ...

public:
    void AddChars(unsigned int charsToAdd) {
        chars_ += charsToAdd;
    }

    // similar for words, images...

    void Display();
};

class DocElement {
    virtual void UpdateStats(DocStats& statistics) = 0;
};

int main() {

}
