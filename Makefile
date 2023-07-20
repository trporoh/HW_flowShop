bin := main
src := main.c

$(bin):$(src)
	gcc $< -o $@ -lpthread