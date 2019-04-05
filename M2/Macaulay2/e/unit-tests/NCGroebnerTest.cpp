#include <memory>
#include <gtest/gtest.h>

#include "engine.h"
#include "poly.hpp"
#include "aring-glue.hpp"
#include "NCAlgebras/FreeAlgebra.hpp"
#include "NCAlgebras/FreeAlgebraQuotient.hpp"
#include "NCAlgebras/WordTable.hpp"
#include "NCAlgebras/NCGroebner.hpp"
#include "NCAlgebras/OverlapTable.hpp"
#include "NCAlgebras/SuffixTree.hpp"
#include <iostream>

std::vector<int> monom1 {2, 0, 1};  // cab
std::vector<int> monom2 {2, 2};  // cc
std::vector<int> monom3 {1, 0, 1, 0};  // baba
std::vector<int> word {2, 0, 1, 2, 2, 1, 0, 1, 0};  // cabccbaba

extern const QQ * globalQQ;

TEST(MonomialOrdering, create)
{
  auto mo1 = MonomialOrderings::Lex(5);
  auto mo2 = MonomialOrderings::GroupLex(4);
  auto mo3 = MonomialOrderings::join({mo1, mo2});
  std::string answer3 { "MonomialOrder => {\n    Lex => 5,\n    GroupLex => 4\n    }" };
  EXPECT_EQ(answer3, MonomialOrderings::toString(mo3));
  EXPECT_EQ(9, rawNumberOfVariables(mo3));
  EXPECT_TRUE(moIsLex(mo1));

  auto mo4 = MonomialOrderings::GRevLex({3,2,5,7});
  EXPECT_TRUE(moIsGRevLex(mo4));
  auto mo5 = MonomialOrderings::GRevLex2({1,1,1,1});
  EXPECT_TRUE(moIsGRevLex(mo5));
  auto mo6 {
    MonomialOrderings::join(
    {
     MonomialOrderings::GRevLex(3),
     MonomialOrderings::GRevLex2(4),
     MonomialOrderings::GRevLex4(5),
     MonomialOrderings::GroupLex(3)
    })};
}

const Monoid* degreeMonoid(const std::vector<std::string>& names)
{
  std::vector<int> wts;
  for (int i=0; i<names.size(); i++)
    wts.push_back(-1);
  MonomialOrdering* mo = MonomialOrderings::join
    ({
      MonomialOrderings::Weights(wts),
      MonomialOrderings::GroupLex(1),
      MonomialOrderings::PositionUp()
    });

  return Monoid::create(mo,
                        names,
                        IM2_Ring_trivial_polyring()->cast_to_PolynomialRing(),
                        {},
                        {});
}

const PolynomialRing* degreeRing(const std::vector<std::string>& names)
{
  auto degM = degreeMonoid(names);
  if (degM == nullptr) return nullptr;
  return PolyRing::create(globalZZ, degM);
}
const PolynomialRing* degreeRing(int ndegrees)
{
  assert(ndegrees == 1);
  return degreeRing({"T"});
}

TEST(FreeAlgebra, create)
{
  if (degreeRing(1) == nullptr or error())
    {
      std::cout << "Error: " << error_message() << std::endl;
      EXPECT_TRUE(false);
    }
  
  FreeAlgebra* A = FreeAlgebra::create(globalQQ,
                                       { "x", "y", "z" },
                                       degreeRing(1),
                                       {1,2,3});
  EXPECT_TRUE(A != nullptr);
}

TEST(FreeAlgebra, polyarithmetic)
{
  FreeAlgebra* A = FreeAlgebra::create(globalQQ,
                                       { "x", "y", "z" },
                                       degreeRing(1),
                                       {1,2,3});
  FreeAlgebraElement x(A), y(A), z(A), f(A), g(A), h(A);
  A->var(*x, 0);
  A->var(*y, 1);
  A->var(*z, 2);
  f = x + y;
  g = y + z;
  EXPECT_TRUE(x + y == y + x);
  EXPECT_FALSE(f == g);
  EXPECT_TRUE(x * (y + z) == x * y + x * z);
  EXPECT_TRUE((f * g) * f == f * (g * f));
  EXPECT_TRUE((f^2) == (f * f));

  A->from_word(*h, {1,2,1,0,1});
  EXPECT_TRUE(h == y * z * y * x * y);

  A->setZero(*f);
  A->subtract(*f,*x,*y);
  EXPECT_TRUE(f == (x - y));

  A->setZero(*f);
  A->setZero(*g);
  A->from_long(*f,1);
  A->from_word(*g,{});
  // from_rational test? How to create an mpq_ptr?
  EXPECT_TRUE(f == g);
  EXPECT_TRUE(A->is_unit(*f));
  EXPECT_TRUE((h^0) == f);

  A->setZero(*f);
  A->setZero(*g);
  A->setZero(*h);
  f = x + y;
  A->from_long(*g,-1);
  A->negate(*h,*f);
  EXPECT_TRUE(h == f*g);
  A->setZero(*h);
  A->mult_by_coeff(*h,*f,A->coefficientRing()->from_long(-1));
  EXPECT_TRUE(h == f*g);

  A->setZero(*f);
  A->setZero(*g);
  A->setZero(*h);
  EXPECT_TRUE(A->is_zero(*h));
  f = x + y;
  A->lead_term_as_poly(*g,*f);
  EXPECT_TRUE(g == x);
  A->add_to_end(*f,*z);
  EXPECT_TRUE(f == (x + y + z));

  A->subtract(*h,*f,*(x+y+z));
  EXPECT_TRUE(A->is_zero(*h));

  A->setZero(*f);
  A->setZero(*g);
  f = x + y;
  A->mult_by_term_left(*g,*f,A->coefficientRing()->from_long(1),Word(monom2));
  EXPECT_TRUE(g == z*z*f);
  A->setZero(*g);
  A->mult_by_term_right(*g,*f,A->coefficientRing()->from_long(1),Word(monom3));
  EXPECT_TRUE(g == f*y*x*y*x);
  A->setZero(*g);
  A->mult_by_term_left_and_right(*g,*f,A->coefficientRing()->from_long(1),Word(monom2),Word(monom3));
  EXPECT_TRUE(g == z*z*f*y*x*y*x);

  // making polynomial monic test
  A->setZero(*f);
  A->setZero(*g);
  A->setZero(*h);
  A->from_long(*f,-1);
  g = f*(x + y);
  A->makeMonic(*h,*g);
  EXPECT_TRUE(h == x + y);
}

TEST(FreeAlgebra, quotientArithmetic)
{
  FreeAlgebra* Q = FreeAlgebra::create(globalQQ,
                                       { "x", "y", "z" },
                                       degreeRing(1),
                                       {1,2,3});
  FreeAlgebraElement X(Q), Y(Q), Z(Q);
  Q->var(*X,0);
  Q->var(*Y,1);
  Q->var(*Z,2);
  
  auto GB = std::unique_ptr<PolyList> (new PolyList);
  GB->push_back(&*(X*Y+Y*X));
  GB->push_back(&*(X*Z+Z*X));
  GB->push_back(&*(Y*Z+Z*Y));
  EXPECT_TRUE(GB->size() == 3);

  FreeAlgebraQuotient* A = new FreeAlgebraQuotient(*Q,std::move(GB));

  FreeAlgebraQuotientElement x(A), y(A), z(A), f(A), g(A), h(A);

  // check if things reduce properly
  A->setZero(*f);
  A->setZero(*g);
  A->setZero(*h);
  f = x*y*x*z*x*y*x*z;
  g = x*x*x*x*y*y*z*z;
  A->negate(*h,*g);
  EXPECT_TRUE(f == g);  
}

TEST(FreeAlgebra, comparisons)
{
  FreeAlgebra* A = FreeAlgebra::create(globalQQ,
                                       { "x", "y", "z" },
                                       degreeRing(1),
                                       {1,2,3});
  FreeAlgebraElement x(A), y(A), z(A), f(A), g(A), h(A);
  A->var(*x, 0);
  A->var(*y, 1);
  A->var(*z, 2);
  EXPECT_TRUE(A->compare_elems(*x,*y) == GT);
  EXPECT_TRUE(A->compare_elems(*y,*x) == LT);
  EXPECT_TRUE(A->compare_elems(*x,*x) == EQ);
}

TEST(FreeAlgebra, spairs)
{
  FreeAlgebra* A = FreeAlgebra::create(globalQQ,
                                       { "x", "y", "z" },
                                       degreeRing(1),
                                       {1,2,3});
  FreeAlgebraElement x(A), y(A), z(A), f(A), g(A), h(A);
  Word leadWord, leadWordPrefix, leadWordSuffix;
  A->var(*x, 0);
  A->var(*y, 1);
  A->var(*z, 2);
  f = x*y*x + z*y*z;
  A->lead_word(leadWord,*f);
  A->lead_word_prefix(leadWordPrefix,*f,2);
  A->lead_word_suffix(leadWordSuffix,*f,1);
  EXPECT_TRUE((*f).cbegin().monom().begin() + 2 == leadWord.begin() && (*f).cbegin().monom().begin() + 5 == leadWord.end());
  EXPECT_TRUE((*f).cbegin().monom().begin() + 2 == leadWordPrefix.begin() && (*f).cbegin().monom().begin() + 4 == leadWordPrefix.end());
  EXPECT_TRUE((*f).cbegin().monom().begin() + 3 == leadWordSuffix.begin() && (*f).cbegin().monom().begin() + 5 == leadWordSuffix.end());

  ConstPolyList polyList {&*f};
  *g = *(NCGroebner::createOverlapPoly(*A, polyList, 0, 2, 0));
  h = f*y*x - x*y*f;
  EXPECT_TRUE(g == h);
}

TEST(OverlapTable, insertion)
{
  OverlapTable overlapTable;
  overlapTable.insert(3,false,std::make_tuple(1,2,3));
  overlapTable.insert(3,false,std::make_tuple(1,2,1));
  overlapTable.insert(2,false,std::make_tuple(1,1,1));
  EXPECT_FALSE(overlapTable.isFinished());
  EXPECT_TRUE(overlapTable.isFinished(1));
  EXPECT_FALSE(overlapTable.isFinished(3));
  EXPECT_TRUE(overlapTable.size() == 3);
}

TEST(NCGroebner, sorting)
{
  auto key1 = std::make_pair(1,false);
  auto key2 = std::make_pair(1,true);
  auto key3 = std::make_pair(2,false);
  auto key4 = std::make_pair(2,true);
  std::map<std::pair<int,bool>,int> myMap;
  myMap.insert(std::make_pair(key1,0));
  myMap.insert(std::make_pair(key2,0));
  myMap.insert(std::make_pair(key3,0));
  myMap.insert(std::make_pair(key4,0));
  // TODO: Make this into a test
  for (auto i : myMap)
    {
      std::cout << "[" << i.first.first << "," << i.first.second << "]" << std::endl;
    }
}

TEST(WordTable, create)
{
  WordTable W;

  EXPECT_EQ(monom1.size(), 3);
  EXPECT_EQ(monom2.size(), 2);

  W.insert(Word(monom1));
  W.insert(Word(monom2));
  W.insert(Word(monom3));

  EXPECT_EQ(W.monomialCount(), 3);
}        

TEST(WordTable, insert)
{
  WordTable W;

  EXPECT_EQ(monom1.size(), 3);
  EXPECT_EQ(monom2.size(), 2);

  W.insert(Word(monom1));
  W.insert(Word(monom2));
  W.insert(Word(monom3));

  std::vector<std::pair<int,int>> matches;
  W.subwords(Word(word), matches);

  EXPECT_EQ(matches.size(), 3);
  EXPECT_EQ(matches[0], std::make_pair(0, 0));
  EXPECT_EQ(matches[1], std::make_pair(1, 3));
  EXPECT_EQ(matches[2], std::make_pair(2, 5));
}        


#if 0
TEST(WordTable, sizet)
{
  size_t a = 3;
  size_t b = 5;
  auto c = a-b;
  std::cout << c << std::endl;
}
#endif

TEST(WordTable, simpleSubwords)
{
  std::vector<int> monom1 {1, 1};  // yy
  std::vector<int> word {1}; // y

  WordTable W;
  W.insert(Word(monom1));
  
  std::vector<std::pair<int,int>> matches;
  W.subwords(Word(word), matches);

  EXPECT_EQ(matches.size(), 0);
}        

TEST(WordTable, subwords)
{
  std::vector<int> monom1 {1, 0, 1, 2};  // babc
  std::vector<int> monom2 {1, 0, 2, 2};  // bacc
  std::vector<int> monom3 {1, 0, 1, 0};  // baba
  std::vector<int> monom4 {1, 0};  // ba
  std::vector<int> word {1, 0, 1, 0, 2, 2, 1, 0, 1, 2};

  WordTable W;

  EXPECT_EQ(monom1.size(), 4);
  EXPECT_EQ(monom2.size(), 4);
  EXPECT_EQ(monom3.size(), 4);
  EXPECT_EQ(monom4.size(), 2);
  EXPECT_EQ(word.size(), 10);
  
  W.insert(Word(monom1));
  W.insert(Word(monom2));
  W.insert(Word(monom3));
  W.insert(Word(monom4));

  std::vector<std::pair<int,int>> matches;
  W.subwords(Word(word), matches);

  EXPECT_EQ(matches.size(), 6);
  EXPECT_EQ(matches[0], std::make_pair(0, 6));
  EXPECT_EQ(matches[1], std::make_pair(1, 2));
  EXPECT_EQ(matches[2], std::make_pair(2, 0));
  EXPECT_EQ(matches[3], std::make_pair(3, 0));
  EXPECT_EQ(matches[4], std::make_pair(3, 2));
  EXPECT_EQ(matches[5], std::make_pair(3, 6));
}        

std::ostream& operator<<(std::ostream& o, const std::vector<Overlap>& val)
{
  int count = 0;
  for (auto a : val)
    {
      o << "[" << std::get<0>(a) << ","
        << std::get<1>(a) << ","
        << std::get<2>(a) << "]";
      o << " ";
      count++;
      if (count % 10 == 0) o << std::endl;
    }
  o << std::endl;
  return o;
}

std::ostream& operator<<(std::ostream& o, const std::vector<std::pair<int,int>>& val)
{
  int count = 0;
  for (auto a : val)
    {
      o << "[" << std::get<0>(a) << ","
        << std::get<1>(a) << "]";
      o << " ";
      count++;
      if (count % 10 == 0) o << std::endl;
    }
  o << std::endl;
  return o;
}

#if 0
template<class T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& val)
{
  int count = 0;
  for (auto a : val)
    {
      o << "[";
      int sz = std::tuple_size<T>::value;
      for (auto i=0; i<sz; ++i)
        {
          o << std::get<i>(a) << ",";
        }
        o << "]";
      o << " ";
      count++;
      if (count % 10 == 0) o << std::endl;
    }
  o << std::endl;
  return o;
}
#endif

TEST(WordTable, skylanin)
{
  // X,Y Z are the 3 variables
#if 0  
  mons = {{Z, X}, {Z, Y}, {Z, Z}, {Y, Y, X}, {Y, Y, Z},
            {Y, X, Y, Y}, {Y, Y, Y, Y}, {Y, X, Y, X, X},
            {Y, X, Y, X, Y}, {Y, X, Y, X, Z},
            {Y, X, X, Y, X, X}, {Y, X, X, Y, X, Z},
          {Y, X, X, Y, Y, Y}}
#endif
          
  std::vector<int> m0 {2,0};  // ZX
  std::vector<int> m1 {2,1};  // ZY
  std::vector<int> m2 {2,2};  // ZZ
  std::vector<int> m3 {1,1,0};  // YYX
  std::vector<int> m4 {1,1,2};  // YYZ
  std::vector<int> m5 {1,0,1,1};  // YXYY
  std::vector<int> m6 {1,1,1,1};  // YYYY
  std::vector<int> m7 {1,0,1,0,0};  // YXYXX
  std::vector<int> m8 {1,0,1,0,1};  // YXYXY
  std::vector<int> m9 {1,0,1,0,2};  // YXYXZ
  std::vector<int> m10 {1,0,0,1,0,0};  // YXXYXX
  std::vector<int> m11 {1,0,0,1,0,2};  // YXXYXZ
  std::vector<int> m12 {1,0,0,1,1,1};  // YXXYYY

  std::vector<Overlap> overlaps;
  std::vector<std::pair<int,int>> matches;
  
  WordTable W;
  W.insert(m0, overlaps);
  EXPECT_EQ(0, overlaps.size());

  W.insert(m1, overlaps);
  EXPECT_EQ(0, overlaps.size());

  W.insert(m2, overlaps);
  std::vector<Overlap> ans {
      std::make_tuple(2,1,0),
      std::make_tuple(2,1,1),
      std::make_tuple(2,1,2)
      };
  EXPECT_EQ(overlaps, ans);
  overlaps.clear();
  W.leftOverlaps(overlaps);
  EXPECT_EQ(0, overlaps.size());

  W.insert(m3);
  W.insert(m4);
  W.insert(m5);
  W.insert(m6);
  W.insert(m7);
  W.insert(m8);
  W.insert(m9);
  W.insert(m10);
  W.insert(m11);
  W.insert(m12);

  matches.clear();
  W.superwords(std::vector<int> {1, 1}, matches);
  std::vector<std::pair<int,int>> ans2
    {
     std::make_tuple(3,0),
     std::make_tuple(4,0),
     std::make_tuple(5,2),
     std::make_tuple(6,0),
     std::make_tuple(6,1),
     std::make_tuple(6,2),
     std::make_tuple(12,3),
     std::make_tuple(12,4)
    };
  EXPECT_EQ(ans2, matches);

  matches.clear();
  EXPECT_TRUE(W.isNontrivialSuperword(std::vector<int> {1,2,1,1,2,1,1}, 6, 6));
  EXPECT_TRUE(W.isNontrivialSuperword(std::vector<int> {1,1,2,1,1,2,1,1,2}, 4, 4));
  
  matches.clear();
  W.subwords(std::vector<int> {2,2,0,1,1,0,1,0,1,1}, matches); // ZZXYYXYXYY
  std::vector<std::pair<int,int>> ans3
    {
     {0,1},
     {2,0},
     {3,3},
     {5,6},
     {8,4}
    };
  EXPECT_EQ(ans3, matches);

}

TEST(WordTable, suffixtree1)
{
  auto vec = std::vector<int> {};
  EXPECT_TRUE(vec.size() == 0);
  SuffixTree* suffixTree = new SuffixTree();
  EXPECT_TRUE(suffixTree->numPatterns() == 0);

  Label s {1,2,3,2,1};
  Label t {1,2,3,4,1};
  Label result {1,2,3};
  EXPECT_TRUE(suffixTree->sharedPrefix(s,t) == result);

  std::vector<Triple> rightOverlaps {};

  auto monList1 = std::vector<Label> { Label {2,2}, Label {2,0,1}, Label {1,0,1,0} };
  suffixTree->insert(monList1, rightOverlaps);
  auto xNode = std::get<0>(suffixTree->extendedLocus(suffixTree->mRoot,Label {0}));
  auto yNode = std::get<0>(suffixTree->extendedLocus(suffixTree->mRoot,Label {1}));
  auto zNode = std::get<0>(suffixTree->extendedLocus(suffixTree->mRoot,Label {2}));
  auto yxNode = std::get<0>(suffixTree->extendedLocus(suffixTree->mRoot,Label {1,0}));
  EXPECT_EQ(xNode->suffixLink(),suffixTree->mRoot);
  EXPECT_EQ(yNode->suffixLink(),suffixTree->mRoot);
  EXPECT_EQ(zNode->suffixLink(),suffixTree->mRoot);
  EXPECT_EQ(yxNode->suffixLink(),xNode);
  auto retval = std::vector<Overlap> { std::make_tuple(0,1,0), std::make_tuple(2,2,2) };
  EXPECT_EQ(rightOverlaps, retval);

  rightOverlaps.clear();
  SuffixTree* suffixTree2 = new SuffixTree();
  auto monList2 = std::vector<Label> {Label {2, 0},Label {2, 1},
                                      Label {2, 2},Label {1, 1, 0},Label {1, 1, 2},
                                      Label {1, 0, 1, 1},Label {1, 1, 1, 1},
                                      Label {1, 0, 1, 0, 0},Label {1, 0, 1, 0, 1},
                                      Label {1, 0, 1, 0, 2},Label {1, 0, 0, 1, 0, 0},
                                      Label {1, 0, 0, 1, 0, 2},Label {1, 0, 0, 1, 1, 1},
                                      Label {1, 0, 0, 0, 1, 0, 1},Label {1, 0, 0, 0, 1, 1, 1},
                                      Label {1, 0, 0, 1, 0, 1, 0},Label {1, 0, 0, 1, 0, 1, 2},
                                      Label {1, 0, 0, 0, 0, 1, 1, 1},Label {1, 0, 0, 0, 1, 0, 0, 0},
                                      Label {1, 0, 0, 0, 1, 0, 0, 1},Label {1, 0, 0, 0, 1, 0, 0, 2},
                                      Label {1, 0, 0, 0, 0, 0, 1, 1, 1},Label {1, 0, 0, 0, 0, 1, 0, 0, 0},
                                      Label {1, 0, 0, 0, 0, 1, 0, 0, 2},Label {1, 0, 0, 0, 0, 1, 0, 1, 0},
                                      Label {1, 0, 0, 0, 0, 1, 0, 1, 2},Label {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
                                      Label {1, 0, 0, 0, 0, 0, 1, 0, 0, 2},Label {1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
                                      Label {1, 0, 0, 0, 0, 0, 1, 0, 1, 2},Label {1, 0, 0, 0, 0, 1, 0, 0, 1, 0},
                                      Label {1, 0, 0, 0, 0, 1, 0, 0, 1, 1},Label {1, 0, 0, 0, 0, 1, 0, 0, 1, 2},
                                      Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2},Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
                                      Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2},Label {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                                      Label {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},Label {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2},
                                      Label {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2},Label {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2},
                                      Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                                      Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2},Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
                                      Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1},Label {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 2}};
  suffixTree2->insert(monList2, rightOverlaps);
  EXPECT_EQ(596,rightOverlaps.size());
  EXPECT_EQ(47,suffixTree2->numPatterns());

  auto subwordsOutput = std::vector<std::pair<int,int>> {};
  suffixTree2->subwords(Label {2,2,0,1,1,0,1,0,1,1}, subwordsOutput);
  auto correctSubwords = std::vector<std::pair<int,int>> { std::make_pair(2,0),
                                                   std::make_pair(0,1),
                                                   std::make_pair(3,3),
                                                   std::make_pair(8,4),
                                                   std::make_pair(5,6) };
  EXPECT_EQ(subwordsOutput,correctSubwords);

  auto subwordOutput = std::make_pair(-1,-1);
  suffixTree2->subword(Label {2,2,0,1,1,0,1,0,1,1}, subwordOutput);
  EXPECT_EQ(subwordOutput, std::make_pair(2,0));
}


// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e/unit-tests runNCGroebnerTest  "
// indent-tabs-mode: nil
// End:
