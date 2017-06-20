answer = http.get "https://vk.com"
if answer.status == 200 and answer.content_type == "text/html
	for link in html(answer.text).get_links:
		say for.num_iteration, link
