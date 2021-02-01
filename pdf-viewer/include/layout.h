#pragma once
#include "layoutable.h"
#include <vector>

class Widget;

class Layout: public Layoutable
{
public:
  enum Style { Vertical, Horizontal };
  Layout(Style);
  void addLayoutable(Layoutable *);
  void removeLayoutable(Layoutable *);
  virtual void setLeft(int) override;
  virtual void setTop(int) override;
  virtual void resize(int width, int height) override;
  virtual int maxHeight() const override;
  virtual int minHeight() const override;
  virtual int maxWidth() const override;
  virtual int minWidth() const override;
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
