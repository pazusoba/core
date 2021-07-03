name=helloworld

all:
	gcc -shared $(name).c -o $(name).so
	dart compile exe $(name).dart -o $(name).out
	./$(name).out

clean:
	rm $(name).so $(name).out
