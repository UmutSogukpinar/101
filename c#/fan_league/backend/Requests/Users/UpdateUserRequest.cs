namespace fan_league.Requests.Users;

public sealed record UpdateUserRequest(
    string Username,
    string Email);
