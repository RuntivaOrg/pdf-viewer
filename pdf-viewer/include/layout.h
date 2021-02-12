#pragma once
#include "layoutable.h"
#include <vector>

class Widget;

class Layout: public Layoutable
{
public:
  enum Style { Vertical, Horizontal };

  Layout(Style);
  ~Layout() final;

  void addLayoutable(Layoutable *);
  void removeLayoutable(Layoutable *);
  void setLeft(int) override;
  void setTop(int) override;
  void resize(int width, int height) override;
  int maxHeight() const override;
  int minHeight() const override;
  int maxWidth() const override;
  int minWidth() const override;
  void setStyle(Style);
  std::vector<Layoutable *> children() const;
private:
  std::vector<Layoutable *> layoutablesList_;
  Style style_;
  int left_;
  int top_;
  int width_;
  int height_;
};
