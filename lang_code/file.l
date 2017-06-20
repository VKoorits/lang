print 3_000_000.14 // 3_000_000.14 eq 3000000.14
/*
multyline comment
and this
and this
*/
file = open "file.txt", "r"
for line in file: 
	say line

answer = http.get "https://vk.com"
if answer.status == 200 and answer.content_type == "text/html"
	for link in html(answer.text).get_links:
		say for.num_iteration, link

