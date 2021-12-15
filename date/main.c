#include <sys/time.h>
#include <buildin/unix_time.h>
#include <stdio.h>

int main() {
	char* month_names[] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};

	long long int unix_time = time();
	
	int year, month, day, hour, minute, second;
	from_unix_time(unix_time, &year, &month, &day, &hour, &minute, &second);

	printf("%d %s %d %d:%d:%d\n", day, month_names[month - 1], year, hour, minute, second);

	return 0;
}