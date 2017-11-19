#include <vector>
#include <iostream>

class DocElementVisitor;

class DocElement {
public:
    virtual void Accept(DocElementVisitor&) = 0;
};



class DocElementVisitor {
public:
    // we need at least one virtual function to allow dynamic_cast
    virtual ~DocElementVisitor() {}
};


class Paragraph;

class ParagraphVisitor {
public:
    virtual void VisitParagraph(Paragraph&) = 0;
};


class RasterBitmap;

class RasterBitmapVisitor {
public:
    virtual void VisitRasterBitmap(RasterBitmap&) = 0;
};

class Paragraph : public DocElement {
    unsigned int fontSize_;
public:
    virtual void Accept(DocElementVisitor& v) {
       if (ParagraphVisitor * p = 
            dynamic_cast<ParagraphVisitor*>(&v)) {
                p->VisitParagraph(*this);
            }
    }
    unsigned int NumChars() {
        return 3; // would count chars here
    }
    unsigned int NumWords() {
        return 5; // woud count words here
    }
    void SetFontSize(unsigned int fontSize) {
        fontSize_ = fontSize;
    }
    unsigned int GetFontSize() const {
        return fontSize_;
    }
};




class RasterBitmap : public DocElement {
public:
    virtual void Accept(DocElementVisitor& v) {
       if (RasterBitmapVisitor * p = 
            dynamic_cast<RasterBitmapVisitor*>(&v)) {
                p->VisitRasterBitmap(*this);
            }
    }
};


class DocStats : public DocElementVisitor, ParagraphVisitor, RasterBitmapVisitor {
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
