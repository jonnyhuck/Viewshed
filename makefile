.PHONY: test testpy framework brew

framework:
	g++ -std=c++11 -stdlib=libc++ viewshed.cpp -F/Library/Frameworks/ -framework GDAL -o viewshed -O3
	swig -python -c++ viewshed.i
	python setup.py build_ext --inplace

brew:
	# g++ viewshed.cpp -oviewshed -L/usr/local/Cellar/gdal/2.4.1_1/lib -I/usr/local/Cellar/gdal/2.4.1_1/include -lgdal -std=c++11 -O3 -v
	swig -python -c++ -Wall -v -O viewshed.i
	export MACOSX_DEPLOYMENT_TARGET=10.14
	python setup.py build_ext --inplace

test:
	./viewshed -r 10000 -x 325000 -y 515000 -z 50 -o 1.5 -t 100 -i data.tif -f VIEWSHED.tif

testpy:
	python test.py
