void exampleFunc() {
  bool some_condition = true;

//! [FieldType Example]
  using MyPT = PackedTuple<BF::Bool, BF::Bool, BF::U32<30>>;

  MyPT::FieldType<2> uIntVal = 0;
  if (some_condition) {
    uIntVal = 1;
  }
//! [FieldType Example]
}