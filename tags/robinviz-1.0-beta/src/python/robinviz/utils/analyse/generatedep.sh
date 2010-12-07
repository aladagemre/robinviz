cd ..
python py2depgraph.py RobinViz.py | python depgraph2dot.py | dot -Kdot -T png -o depgraph.png
sfood confirmation.py core.py drawing.py extensions.py multiview.py singleview.py windows.py | sfood-graph > test3.dot
cat test3.dot | dot -Kdot -Tpng > test3.png
pycallgraph ./RobinViz.py

