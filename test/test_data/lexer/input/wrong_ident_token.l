//print 3_000_000.14.16

file = open "file.txt", "r"
for _line in file: 
//	say 789line
	say _line
answer = http.get "https://vk.com"
if answer.status == 200 and answer.content_type == "text/html"
	for 1link in html(answer.text).get_links:
		say for.num_iteration, 1link
