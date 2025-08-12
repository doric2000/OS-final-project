QUESTION_DIRS = Question1 Question2 Question3 Question4 Question6 Question7 Question8 Question9 Question10 Question11

all: $(QUESTION_DIRS)

$(QUESTION_DIRS):
	$(MAKE) -C $@

clean:
	$(MAKE) -C Question1 clean
	$(MAKE) -C Question2 clean
	$(MAKE) -C Question3 clean
	$(MAKE) -C Question4 clean
	$(MAKE) -C Question6 clean
	$(MAKE) -C Question7 clean
	$(MAKE) -C Question8 clean
	$(MAKE) -C Question9 clean
	$(MAKE) -C Question10 clean
	$(MAKE) -C Question11 clean

.PHONY: all clean $(QUESTION_DIRS)
