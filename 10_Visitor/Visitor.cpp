#include <vector>
#include <iostream>

class DocElementVisitor;

class DocElement {
public:
    virtual void Accept(DocElementVisitor&) = 0;
};

class Paragraph;
class RasterBitmap;

class DocElementVisitor {
public:
    virtual void VisitParagraph(Paragraph&) = 0;
    virtual void VisitRasterBitmap(RasterBitmap&) = 0;
    // ...
};

class Paragraph : public DocElement {
public:
        unsigned int NumChars() {
            return 3; // would count chars here
        }
        unsigned int NumWords() {
            return 5; // woud count words here
        }
    virtual void Accept(DocElementVisitor& v) {
        v.VisitParagraph(*this);
    }
};


class RasterBitmap : public DocElement {
public:
    virtual void Accept(DocElementVisitor& v) {
        v.VisitRasterBitmap(*this);
    }
};


class DocStats : public DocElementVisitor {
    unsigned int chars_, words_, images_;
public:
    virtual void VisitParagraph(Paragraph& par) {
        chars_ += par.NumChars();
        words_ += par.NumWords();
    }

    virtual void VisitRasterBitmap(RasterBitmap&) {
        ++images_;
    }

    void Display() {
        std::cout << "Stats:\n";
        std::cout << chars_ << "\n";
        std::cout << words_ << "\n";
        std::cout << images_ << "\n\n";
    }
};


class Document {
std::vector<DocElement> docElements_;

    void DisplayStatistics() {
        DocStats statistics;
        std::vector<DocElement>::iterator it;
        for (it = docElements_.begin(); it != docElements_.end(); ++it) {
            it->Accept(statistics);
        }
        statistics.Display();
    }
};


int main() {

}
